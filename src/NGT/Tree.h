//
// Copyright (C) 2015-2017 Yahoo Japan Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//    http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#pragma once

#include	"NGT/defines.h"
#include	"NGT/Common.h"
#include	"NGT/Node.h"

#include	<string>
#include	<vector>
#include	<stack>

namespace NGT {

  class DVPTree {

  public:
    enum SplitMode {
      MaxDistance	= 0,
      MaxVariance	= 1
    };

    typedef vector<Node::ID>	IDVector;

    class Container : public NGT::Container {
    public:
      Container(Object &f, ObjectID i):NGT::Container(f, i) {}
      DVPTree			*vptree;
    };

    class SearchContainer : public NGT::SearchContainer {
    public:
      enum Mode {
	SearchLeaf	= 0,
	SearchObject	= 1
      };

      SearchContainer(Object &f, ObjectID i):NGT::SearchContainer(f, i) {}
      SearchContainer(Object &f):NGT::SearchContainer(f, 0) {}

      DVPTree			*vptree;

      Mode		mode;
      Node::ID	nodeID;
    };
    class InsertContainer : public Container {
    public:
      InsertContainer(Object &f, ObjectID i):Container(f, i) {}
    };

    class RemoveContainer : public Container {
    public:
      RemoveContainer(Object &f, ObjectID i):Container(f, i) {}
    };

    DVPTree() {
      leafObjectsSize = LeafNode::LeafObjectsSizeMax;
      internalChildrenSize = InternalNode::InternalChildrenSizeMax;
      splitMode = MaxVariance;
#ifndef NGT_SHARED_MEMORY_ALLOCATOR
      insertNode(new LeafNode);
#endif
    }

    virtual ~DVPTree() {
#ifndef NGT_SHARED_MEMORY_ALLOCATOR
      deleteAll();
#endif
    }

    void deleteAll() {
      for (size_t i = 0; i < leafNodes.size(); i++) {
	if (leafNodes[i] != 0) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  leafNodes[i]->deletePivot(*objectSpace, leafNodes.allocator);
#else
	  leafNodes[i]->deletePivot(*objectSpace);
#endif
	  delete leafNodes[i];
	}
      }
      leafNodes.clear();
      for (size_t i = 0; i < internalNodes.size(); i++) {
	if (internalNodes[i] != 0) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  internalNodes[i]->deletePivot(*objectSpace, internalNodes.allocator);
#else
	  internalNodes[i]->deletePivot(*objectSpace);
#endif
	  delete internalNodes[i];
	}
      }
      internalNodes.clear();
    }

#ifdef NGT_SHARED_MEMORY_ALLOCATOR
    void open(const string &f, size_t sharedMemorySize) {
      // If no file, then create a new file.
      leafNodes.open(f + "l", sharedMemorySize);
      internalNodes.open(f + "i", sharedMemorySize);
      if (leafNodes.size() == 0) {
	if (internalNodes.size() != 0) {
          NGTThrowException("Tree::Open: Internal error. Internal and leaf are inconsistent.");
	}
	LeafNode *ln = leafNodes.allocate();
	insertNode(ln);
      }
    }
#endif // NGT_SHARED_MEMORY_ALLOCATOR

    void insert(InsertContainer &iobj);

    void insert(InsertContainer &iobj, LeafNode *n);

    Node::ID split(InsertContainer &iobj, LeafNode &leaf);

    Node::ID recombineNodes(InsertContainer &ic, Node::Objects &fs, LeafNode &leaf);

    void insertObject(InsertContainer &obj, LeafNode &leaf);

    typedef stack<Node::ID> UncheckedNode;

    void search(SearchContainer &so);
    void search(SearchContainer &so, InternalNode &node, UncheckedNode &uncheckedNode);
    void search(SearchContainer &so, LeafNode &node, UncheckedNode &uncheckedNode);

    void replace(ObjectID id, ObjectID replacedId) { remove(id, replacedId); }
    
    // remove the specified object.
    void remove(ObjectID id, ObjectID replaceId = 0) {
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      Object *qobject = objectSpace->allocateObject(*getObjectRepository().get(id));
      SearchContainer q(*qobject);
#else
      SearchContainer q(*getObjectRepository().get(id));
#endif
      q.mode = SearchContainer::SearchLeaf;
      q.vptree = this;
      q.radius = 0.0;
      q.size = 5;

      search(q);

#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      objectSpace->deleteObject(qobject);
#endif

      LeafNode &ln = *(LeafNode*)getNode(q.nodeID);
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      ln.removeObject(id, replaceId, leafNodes.allocator);
#else
      ln.removeObject(id, replaceId);
#endif
      if (ln.getObjectSize() == 0) {
	if (ln.parent.getID() != 0) {
	  InternalNode &inode = *(InternalNode*)getNode(ln.parent);
	  removeEmptyNodes(inode);
	}
      }

      return;
    }

    Node *getRootNode() {
      size_t nid = 1;
      Node *root;
      try {
  	root = internalNodes.get(nid);
      } catch(Exception &err) {
        try {
  	  root = leafNodes.get(nid);
        } catch(Exception &e) {
          stringstream msg;
          msg << "VpTree::getRootNode: Inner error. Cannot get a leaf root node. " << nid << ":" << e.what();
          NGTThrowException(msg);
        }
      }

      return root;
    }

    InternalNode *createInternalNode() {
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      InternalNode *n = new(internalNodes.allocator) InternalNode(internalChildrenSize, internalNodes.allocator);
#else
      InternalNode *n = new InternalNode(internalChildrenSize);
#endif
      insertNode(n);
      return n;
    }

    void
      removeNode(Node::ID id) {
      size_t idx = id.getID();
      if (id.getType() == Node::ID::Leaf) {
	leafNodes.remove(idx);
      } else {
	internalNodes.remove(idx);	  
      }
    }

    void removeEmptyNodes(InternalNode &node);

    Node::Objects * getObjects(LeafNode	&n, Container	&iobj);

    void getObjectIDsFromLeaf(Node::ID		nid,      ObjectDistances	&rl) {
      LeafNode &ln = *(LeafNode*)getNode(nid);
      rl.clear();
      ObjectDistance	r;
      for (size_t i = 0; i < ln.getObjectSize(); i++) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
        r.id = ln.getObjectIDs(leafNodes.allocator)[i].id;
        r.distance = ln.getObjectIDs(leafNodes.allocator)[i].distance;
#else
        r.id = ln.getObjectIDs()[i].id;
        r.distance = ln.getObjectIDs()[i].distance;
#endif
        rl.push_back(r);
      }
      return;
    }
    void
      insertNode(LeafNode *n) {
      size_t id = leafNodes.insert(n);
      n->id.setID(id);
      n->id.setType(Node::ID::Leaf);
    }

    // replace
    void replaceNode(LeafNode *n) {
      int id = n->id.getID();
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      leafNodes.set(id, n);
#else
      leafNodes[id] = n;
#endif
    }

    void
      insertNode(InternalNode *n) 
    {
      size_t id = internalNodes.insert(n);
      n->id.setID(id);
      n->id.setType(Node::ID::Internal);
    }

    Node *getNode(Node::ID &id) {
      Node *n = 0;
      Node::NodeID idx = id.getID();
      if (id.getType() == Node::ID::Leaf) {
	n = leafNodes.get(idx);
      } else {
	n = internalNodes.get(idx);
      }
      return n;
    }

    void serialize(ofstream &os) {
      leafNodes.serialize(os, objectSpace);
      internalNodes.serialize(os, objectSpace);
    }

    void deserialize(ifstream &is) {
      leafNodes.deserialize(is, objectSpace);
      internalNodes.deserialize(is, objectSpace);
    }

    void serializeAsText(ofstream &os) {
      leafNodes.serializeAsText(os, objectSpace);
      internalNodes.serializeAsText(os, objectSpace);
    }

    void deserializeAsText(ifstream &is) {
      leafNodes.deserializeAsText(is, objectSpace);
      internalNodes.deserializeAsText(is, objectSpace);
    }

    void show() {
      cout << "Show tree " << endl;
      for (size_t i = 0; i < leafNodes.size(); i++) {
	if (leafNodes[i] != 0) {
	  cout << i << ":";
	  (*leafNodes[i]).show();
	}
      }
      for (size_t i = 0; i < internalNodes.size(); i++) {
	if (internalNodes[i] != 0) {
	  cout << i << ":";
	  (*internalNodes[i]).show();
	}
      }
    }

    void deleteInMemory() {
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      assert(0);
#else
      for (vector<NGT::LeafNode*>::iterator i = leafNodes.begin(); i != leafNodes.end(); i++) {
	if ((*i) != 0) {
	  delete (*i);
	}
      }
      leafNodes.clear();
      for (vector<NGT::InternalNode*>::iterator i = internalNodes.begin(); i != internalNodes.end(); i++) {
	if ((*i) != 0) {
	  delete (*i);
	}
      }
      internalNodes.clear();
#endif
    }

    ObjectRepository &getObjectRepository() { return objectSpace->getRepository(); }


  public:
    size_t		internalChildrenSize;
    size_t		leafObjectsSize;

    SplitMode		splitMode;

    string		name;

#ifdef NGT_SHARED_MEMORY_ALLOCATOR
    PersistentRepository<LeafNode>		leafNodes;
    PersistentRepository<InternalNode>		internalNodes;
#else
    Repository<LeafNode>	leafNodes;
    Repository<InternalNode>	internalNodes;
#endif

    ObjectSpace		*objectSpace;

  };
} // namespace DVPTree


