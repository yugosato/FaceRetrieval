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

#include	"defines.h"
#include	"Graph.h"
#include	"Thread.h"
#include	"Index.h"


using namespace NGT;

void 
NeighborhoodGraph::Property::set(NGT::Property &prop) {
  if (prop.truncationThreshold != -1)		truncationThreshold = prop.truncationThreshold;
  if (prop.edgeSizeForCreation != -1)		edgeSizeForCreation = prop.edgeSizeForCreation;
  if (prop.edgeSizeForSearch != -1)		edgeSizeForSearch = prop.edgeSizeForSearch;
  if (prop.edgeSizeLimitForCreation != -1)	edgeSizeLimitForCreation = prop.edgeSizeLimitForCreation;
  if (prop.insertionRadiusCoefficient != -1)	insertionRadiusCoefficient = prop.insertionRadiusCoefficient;
  if (prop.seedSize != -1)			seedSize = prop.seedSize;
  if (prop.seedType != SeedTypeNone)		seedType = prop.seedType;
  if (prop.truncationThreadPoolSize != -1)	truncationThreadPoolSize = prop.truncationThreadPoolSize;
  if (prop.batchSizeForCreation != -1)		batchSizeForCreation = prop.batchSizeForCreation;
  if (prop.graphType != GraphTypeNone)		graphType = prop.graphType;
}

void 
NeighborhoodGraph::Property::get(NGT::Property &prop) {
  prop.truncationThreshold 		= truncationThreshold;
  prop.edgeSizeForCreation		= edgeSizeForCreation;
  prop.edgeSizeForSearch		= edgeSizeForSearch;
  prop.edgeSizeLimitForCreation		= edgeSizeLimitForCreation;
  prop.insertionRadiusCoefficient	= insertionRadiusCoefficient;
  prop.seedSize				= seedSize;
  prop.seedType				= seedType;
  prop.truncationThreadPoolSize		= truncationThreadPoolSize;
  prop.batchSizeForCreation		= batchSizeForCreation;
  prop.graphType			= graphType;
}

void 
NeighborhoodGraph::setupSeeds(NGT::SearchContainer &sc, ObjectDistances &seeds, ResultSet &result, 
			      UncheckedSet &unchecked, DistanceCheckedSet &distanceChecked)
{

  ObjectDistances tmp;
  for (ObjectDistances::iterator i = seeds.begin(); i != seeds.end(); i++) {
    ObjectDistance result;
    result.id = (*i).id;
    if (getObjectRepository().isEmpty(result.id)) {
      cerr << "setupseeds:fatal error " << result.id << endl;
      assert(0);
    }

    NGT::ObjectSpace::neuralWeight = sc.weight;
    NGT::ObjectSpace::neuralBias = sc.bias;

    Distance d = objectSpace->getComparator()(sc.object, *getObjectRepository().get(result.id));
    result.distance = d;
    tmp.push_back(result);
  }
  std::sort(tmp.begin(), tmp.end());

  if (tmp.size() > (size_t)property.seedSize) {
    tmp.resize(property.seedSize);
  }

#ifdef NGT_GRAPH_UNCHECK_STACK
  for (ObjectDistances::reverse_iterator ri = tmp.rbegin(); ri != tmp.rend(); ri++) {
#else
  for (ObjectDistances::iterator ri = tmp.begin(); ri != tmp.end(); ri++) {
#endif
#if !defined(NGT_GRAPH_CHECK_VECTOR) || defined(NGT_GRAPH_CHECK_BOOLEANSET)
    distanceChecked.insert((*ri).id);
#else
    distanceChecked[(*ri).id] = 1;
#endif
    unchecked.push(*ri);
  }
  assert(sc.size > 0);
  assert(sc.radius >= 0.0);
  for (ObjectDistances::iterator ri = tmp.begin(); ri != tmp.end(); ri++) {
    if ((result.size() < (unsigned int)sc.size) && ((*ri).distance <= sc.radius)) {
      result.push((*ri));
    } else {
      break;
    }
  }

}

  void
    NeighborhoodGraph::search(NGT::SearchContainer &sc, ObjectDistances &seeds)
  {
    if (sc.explorationCoefficient == 0.0) {
      sc.explorationCoefficient = NGT_EXPLORATION_COEFFICIENT;
    }

    if (sc.getResult().size() != 0) {
      cerr << "Graph::search: Warning : A set of the resuts is not empty" << endl;
    }
    UncheckedSet unchecked;

    ResultSet results;

#ifdef NGT_GRAPH_CHECK_VECTOR
#if defined(NGT_GRAPH_CHECK_BITSET)
    DistanceCheckedSet distanceChecked(0);
#elif defined(NGT_GRAPH_CHECK_BOOLEANSET)
    DistanceCheckedSet distanceChecked(repository.size() + 1);
#else
    DistanceCheckedSet distanceChecked(repository.size() + 1, 0);
#endif
#else // NGT_GRAPH_CHECK_VECTOR
    DistanceCheckedSet distanceChecked;
#endif  // NGT_GRAPH_CHECK_VECTOR

    setupSeeds(sc, seeds, results, unchecked, distanceChecked);
    Distance explorationRadius = sc.explorationCoefficient * sc.radius;

    NGT::ObjectSpace::neuralWeight = sc.weight;
    NGT::ObjectSpace::neuralBias = sc.bias;

    NGT::ObjectSpace::Comparator &comparator = objectSpace->getComparator();
    while (!unchecked.empty()) {
      ObjectDistance target = unchecked.top();
      unchecked.pop();
      if (target.distance > explorationRadius) {
#ifdef NGT_GRAPH_UNCHECK_STACK
	continue;
#else
	break;
#endif
      }
      GraphNode *rsptr = 0;
      try {
	rsptr = getNode(target.id);
      } catch(Exception &err) {
	cerr << "Graph::search: Warning. " << err.what() << "  ID=" << target.id << endl;
	continue;
      }
      GraphNode &neighbors = *rsptr;
      if (neighbors.size() == 0) {
	// When the only one object is in the DB, the node has no neighbors.
	continue;
      }
#ifdef NGT_GRAPH_UNCHECK_STACK_SORT
      ObjectDistances sort;
#endif
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      ObjectDistance *neighborptr = &neighbors.at(0, repository.allocator);
#else
      ObjectDistance *neighborptr = &neighbors[0];
#endif
      ObjectDistance *neighborendptr = neighborptr;
      if (property.edgeSizeForSearch == 0) {
	neighborendptr += neighbors.size();
      } else {
	neighborendptr += (neighbors.size() < (size_t)property.edgeSizeForSearch ? neighbors.size() : (size_t)property.edgeSizeForSearch);
      }
      for (; neighborptr < neighborendptr; ++neighborptr) {
	ObjectDistance &neighbor = *neighborptr;
#ifdef NGT_GRAPH_CHECK_VECTOR
	if (distanceChecked[neighbor.id]) {
	  continue;
	}
#ifdef NGT_GRAPH_CHECK_BOOLEANSET
	distanceChecked.insert(neighbor.id);
#else
	distanceChecked[neighbor.id] = true;
#endif
#else // NGT_GRAPH_CHECK_VECTOR
	if (distanceChecked.find(neighbor.id) != distanceChecked.end()) {
	  continue;
	}
	distanceChecked.insert(neighbor.id);
#endif // NGT_GRAPH_CHECK_VECTOR

#ifdef NGT_EXPLORATION_COEFFICIENT_OPTIMIZATION
	sc.explorationCoefficient = exp(-(double)distanceChecked.size() / 20000.0) / 10.0 + 1.0;
#endif

	NGT::ObjectSpace::neuralWeight = sc.weight;
	NGT::ObjectSpace::neuralBias = sc.bias;

	Distance distance = comparator(sc.object, *getObjectRepository().get(neighbor.id));
	ObjectDistance r(neighbor.id, distance);
	if (distance <= explorationRadius) {
#ifdef NGT_GRAPH_UNCHECK_STACK
#ifdef NGT_GRAPH_UNCHECK_STACK_SORT
	  sort.push_back(r);
#else
	  unchecked.push(r);
#endif
#else
	  unchecked.push(r);
#endif // NGT_GRAPH_UNCHECK_STACK
	  if (distance <= sc.radius) {
	    results.push(r);
	    // This part has been already optimized. No space to improve.
	    if (results.size() > sc.size) {
	      results.pop();
	    } 
	    if (results.size() == sc.size) {
	      sc.radius = results.top().distance;
	      explorationRadius = sc.explorationCoefficient * sc.radius;
	    }
	  } 
	} 
      } 

#ifdef NGT_GRAPH_UNCHECK_STACK_SORT
      // sort is not effectictive.
      std::sort(sort.begin(), sort.end());
      for (ObjectDistances::reverse_iterator si = sort.rbegin(); si != sort.rend(); si++) {
	unchecked.push(*si);
      }
#endif
    } 
    {
      ObjectDistances &qresults = sc.getResult();
      qresults.moveFrom(results);
    }

  }

  void
  NeighborhoodGraph::removeEdgesReliably(ObjectID id) {
    cerr << "Graph::removeEdgesReliably" << endl;
    GraphNode *nodetmp = 0;
    try {
      nodetmp = getNode(id);
    } catch (Exception &err) {
      stringstream msg;
      msg << "removeEdgesReliably : cannot find a node. ID=" << id;
      msg << ":" << err.what();
      NGTThrowException(msg.str());
    }
    if (nodetmp == 0) {
      stringstream msg;
      msg << "removeEdgesReliably : cannot find a node. ID=" << id;
      NGTThrowException(msg.str());
    }
    GraphNode &node = *nodetmp;
    if (node.size() == 0) {
      cerr << "removeEdgesReliably : Warning! : No edges. ID=" << id << endl;
      try {
	removeNode(id);
      } catch (Exception &err) {
	stringstream msg;
	msg << "removeEdgesReliably : Internal error! : cannot remove a node without edges. ID=" << id;
	msg << ":" << err.what();
	NGTThrowException(msg.str());
      }
      return;
    }

    vector<PersistentObject*> objtbl;
    vector<GraphNode*> nodetbl;
    try {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      for (GraphNode::iterator i = node.begin(repository.allocator); i != node.end(repository.allocator);) {
#else
      for (GraphNode::iterator i = node.begin(); i != node.end();) {
#endif
	if (id == (*i).id) {
	  cerr << "Graph::removeEdgesReliably: Inner error. Destination nodes include a source node. ID="
	       << id << " continue..." << endl;
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  i = node.erase(i, repository.allocator);
#else
	  i = node.erase(i);
#endif
	  continue;
	}
	objtbl.push_back(getObjectRepository().get((*i).id));
	GraphNode *n = 0;
	try {
	  n = getNode((*i).id);
	} catch (Exception &err) {
	  cerr << "Graph::removeEdgesReliably: Cannot find edges of a child. ID="
	       << (*i).id << " continue..." << endl;
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  i = node.erase(i, repository.allocator);
#else
	  i = node.erase(i);
#endif
	  continue;
	}
	nodetbl.push_back(n);

	ObjectDistance edge;
	edge.id = id;
	edge.distance = (*i).distance;
	{
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  GraphNode::iterator ei = std::lower_bound(n->begin(repository.allocator), n->end(repository.allocator), edge);
	  if (ei != n->end(repository.allocator) && (*ei).id == id) {
	    n->erase(ei, repository.allocator);
#else
	  GraphNode::iterator ei = std::lower_bound(n->begin(), n->end(), edge);
	  if (ei != n->end() && (*ei).id == id) {
	    n->erase(ei);
#endif
	  } else {
	    stringstream msg;
	    msg << "removeEdgesReliably : internal error : cannot find an edge. ID="
		<< id << " d=" << edge.distance << " in " << (*i).id << endl;
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	    for (GraphNode::iterator ni = n->begin(repository.allocator); ni != n->end(repository.allocator); ni++) {
#else
	    for (GraphNode::iterator ni = n->begin(); ni != n->end(); ni++) {
#endif
	      msg << "check. " << (*ni).id << endl;
	    }
#ifdef NGT_FORCED_REMOVE
	    msg << " anyway continue...";
	    cerr << msg.str() << endl;
#else
	    NGTThrowException(msg.str());
#endif
	  }
	}
	i++;
      }

      for (unsigned int i = 0; i < node.size() - 1; i++) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	assert(node.at(i, repository.allocator).id != id);
#else
	assert(node[i].id != id);
#endif
	int minj = -1;
	Distance mind = FLT_MAX;
	for (unsigned int j = i + 1; j < node.size(); j++) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  assert(node.at(j, repository.allocator).id != id);
#else
	  assert(node[j].id != id);
#endif
	  Distance d = objectSpace->getComparator()(*objtbl[i], *objtbl[j]);
	  if (d < mind) {
	    minj = j;
	    mind = d;
	  }
	}
	assert(minj != -1);
	{
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  ObjectDistance obj = node.at(minj, repository.allocator);
#else
	  ObjectDistance obj = node[minj];
#endif
	  obj.distance = mind;
	  GraphNode &n = *nodetbl[i];
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  GraphNode::iterator ei = std::lower_bound(n.begin(repository.allocator), n.end(repository.allocator), obj);
	  if (ei == n.end(repository.allocator) || (*ei).id != obj.id) {
	    n.insert(ei, obj, repository.allocator);
	  }
#else
	  GraphNode::iterator ei = std::lower_bound(n.begin(), n.end(), obj);
	  if (ei == n.end() || (*ei).id != obj.id) {
	    n.insert(ei, obj);
	  }
#endif
	}
	{
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  ObjectDistance obj = node.at(i, repository.allocator);
#else
	  ObjectDistance obj = node[i];
#endif
	  obj.distance = mind;
	  GraphNode &n = *nodetbl[minj];
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  GraphNode::iterator ei = std::lower_bound(n.begin(repository.allocator), n.end(repository.allocator), obj);
	  if (ei == n.end(repository.allocator) || (*ei).id != obj.id) {
	    n.insert(ei, obj, repository.allocator);
#else
	  GraphNode::iterator ei = std::lower_bound(n.begin(), n.end(), obj);
	  if (ei == n.end() || (*ei).id != obj.id) {
	    n.insert(ei, obj);
#endif
	  }
	}
	if ((i + 1 < node.size()) && (i + 1 != (unsigned int)minj)) {
	  ObjectDistance tmpr;
	  PersistentObject *tmpf;
	  GraphNode *tmprs;

#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  tmpr = node.at(i + 1, repository.allocator);
#else
	  tmpr = node[i + 1];
#endif
	  tmpf = objtbl[i + 1];
	  tmprs = nodetbl[i + 1];

#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  node.at(i + 1, repository.allocator) = node.at(minj, repository.allocator);
#else
	  node[i + 1] = node[minj];
#endif
	  objtbl[i + 1] = objtbl[minj];
	  nodetbl[i + 1] = nodetbl[minj];

#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  node.at(minj, repository.allocator) = tmpr;
#else
	  node[minj] = tmpr;
#endif
	  objtbl[minj] = tmpf;
	  nodetbl[minj] = tmprs;
	}
      } // for (unsigned int i = 0; i < results.size() - 1; i++) {

    } catch(Exception &err) {
      stringstream msg;
      msg << "removeEdgesReliably : Relink error ID=" << id << ":" << err.what();
#ifdef NGT_FORCED_REMOVE
      cerr << msg.str() << " continue..." << endl;
#else
      NGTThrowException(msg.str());
#endif
    }

    try {
      removeNode(id);
    } catch (Exception &err) {
      stringstream msg;
      msg << "removeEdgesReliably : removeEdges error. ID=" << id << ":" << err.what();
      NGTThrowException(msg.str());
    }
  }

class TruncationSearchJob {
public:
  TruncationSearchJob() {}
  TruncationSearchJob &operator=(TruncationSearchJob &d) {
    idx = d.idx;
    object = d.object;
    nearest = d.nearest;
    start = d.start;
    radius = d.radius;
    return *this;
  }
  size_t		idx;
  PersistentObject	*object;
  ObjectDistance	nearest;
  ObjectDistance	start;
  NGT::Distance	radius;
};

class TruncationSearchSharedData {
public:
  TruncationSearchSharedData(NGT::NeighborhoodGraph &g, NGT::ObjectID id, size_t size, NGT::Distance lr) : 
    graphIndex(g), targetID(id), resultSize(size), explorationCoefficient(lr) {}
  NGT::NeighborhoodGraph	&graphIndex;
  NGT::ObjectID		targetID;
  size_t			resultSize;
  NGT::Distance		explorationCoefficient;
};

class TruncationSearchThread : public NGT::Thread {
public:
  TruncationSearchThread() {}
  virtual ~TruncationSearchThread() {}
  virtual int run() {
    NGT::ThreadPool<TruncationSearchJob, TruncationSearchSharedData*, TruncationSearchThread>::Thread &poolThread =
      (NGT::ThreadPool<TruncationSearchJob, TruncationSearchSharedData*, TruncationSearchThread>::Thread&)*this;
    TruncationSearchSharedData &sd = *poolThread.getSharedData();
    for (;;) {
      TruncationSearchJob job;
      try {
	poolThread.getInputJobQueue().popFront(job);
      } catch(NGT::ThreadTerminationException &err) {
	break;
      } catch (NGT::Exception &err) {
	cerr << "TruncationSearchThread::run()::Inner error. continue..." << endl;
      }
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      Object *po = sd.graphIndex.objectSpace->allocateObject((Object&)*job.object);
      NGT::SearchContainer ssc(*po);
#else
      NGT::SearchContainer ssc(*job.object);
#endif

      NGT::ObjectDistances srs, results;

      srs.push_back(job.start);
      ssc.setResults(&results);
      ssc.size = sd.resultSize;
      ssc.radius = job.radius;
      ssc.explorationCoefficient = sd.explorationCoefficient;
      ssc.id = 0;
      try {
	sd.graphIndex.search(ssc, srs);
      } catch(...) {
	cerr << "CreateIndexThread::run : Fatal Error!" << endl;
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
	sd.graphIndex.objectSpace->deleteObject(po);
#endif
      }
#ifdef NGT_SHARED_MEMORY_ALLOCATOR
      sd.graphIndex.objectSpace->deleteObject(po);
#endif
      job.nearest = results[0];
      poolThread.getOutputJobQueue().pushBack(job);
    }
    return 0;
  }

};

typedef NGT::ThreadPool<TruncationSearchJob, TruncationSearchSharedData*, TruncationSearchThread> TruncationSearchThreadPool;

int
NeighborhoodGraph::truncateEdgesOptimally(
					  ObjectID id,
					  GraphNode &results,
					  size_t truncationSize
					  )
{

  ObjectDistances delNodes;

  size_t osize = results.size();

  size_t resSize = 2;
  TruncationSearchThreadPool threads(property.truncationThreadPoolSize);
  TruncationSearchSharedData sd(*this, id, resSize, 1.1);
  threads.setSharedData(&sd);
  threads.create();

  try {
    for (size_t i = truncationSize; i < results.size(); i++) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      if (id == results.at(i, repository.allocator).id) {
#else
      if (id == results[i].id) {
#endif
	continue;
      }
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      delNodes.push_back(results.at(i, repository.allocator));
#else
      delNodes.push_back(results[i]);
#endif
    }

#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
    GraphNode::iterator ri = results.begin(repository.allocator);
    ri += truncationSize;
    results.erase(ri, results.end(repository.allocator), repository.allocator);
#else
    GraphNode::iterator ri = results.begin();
    ri += truncationSize;
    results.erase(ri, results.end());
#endif

    for (size_t i = 0; i < delNodes.size(); i++) {
      GraphNode::iterator j;
      GraphNode &res = *getNode(delNodes[i].id);
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
      for (j = res.begin(repository.allocator); j != res.end(repository.allocator); j++) {
#else
      for (j = res.begin(); j != res.end(); j++) {
#endif
	if ((*j).id == id) {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	  res.erase(j, repository.allocator);
#else
	  res.erase(j);
#endif
	  break;
	}
      }
    }
    bool retry = true;
    size_t maxResSize = osize * 2;
    size_t batchSize = 20;
    TruncationSearchThreadPool::OutputJobQueue &output = threads.getOutputJobQueue();
    TruncationSearchJob job;

    for (; retry == true; resSize = maxResSize) {
      retry = false;
      sd.resultSize = resSize;
      size_t nodeidx = 0;
      for (;;) {
	size_t nodeSize = 0;
	for (; nodeidx < delNodes.size(); nodeidx++) {
	  if (delNodes[nodeidx].id == 0) {
	    continue;
	  }
	  nodeSize++;
	  job.object = getObjectRepository().get(delNodes[nodeidx].id);
	  job.idx = nodeidx;
	  job.start.id = id;
	  job.start.distance = delNodes[nodeidx].distance;
	  job.radius = FLT_MAX;
	  threads.pushInputQueue(job);
	  if (nodeSize >= batchSize) {
	    break;
	  }
	}
	if (nodeSize == 0) {
	  break;
	}
	threads.waitForFinish();

	if (output.size() != nodeSize) {
	  nodeSize = output.size();
	}
	size_t cannotMoveCnt = 0;
	for (size_t i = 0; i < nodeSize; i++) {
	  TruncationSearchJob &ojob = output.front();
	  ObjectID nearestID = ojob.nearest.id;
	  size_t idx = ojob.idx;
	  if (nearestID == delNodes[idx].id) {
	    delNodes[idx].id = 0;
	    output.pop_front();
	    continue;
	  } else if (nearestID == id) {
	    cannotMoveCnt++;
	    if ((resSize < maxResSize) && (cannotMoveCnt > 1)) {
	      retry = true;
	      output.pop_front();
	      continue;
	    }
	  } else {
	  }

	  ObjectID tid = delNodes[idx].id;
	  delNodes[idx].id = 0;

	  GraphNode &delres = *getNode(tid);
	  {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	    GraphNode::iterator ei = std::lower_bound(delres.begin(repository.allocator), delres.end(repository.allocator), ojob.nearest);
	    if (ei == delres.end(repository.allocator) || (*ei).id != ojob.nearest.id) {
	      delres.insert(ei, ojob.nearest, repository.allocator);
#else
	    GraphNode::iterator ei = std::lower_bound(delres.begin(), delres.end(), ojob.nearest);
	    if (ei == delres.end() || (*ei).id != ojob.nearest.id) {
	      delres.insert(ei, ojob.nearest);
#endif
	    } else {
	      output.pop_front();
	      continue;
	    }
	  }
	  ObjectDistance r;
	  r.distance = ojob.nearest.distance;
	  r.id = tid;
	  if (nearestID != id) {
	    GraphNode &rs = *getNode(nearestID);
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	    rs.push_back(r, repository.allocator);
	    std::sort(rs.begin(repository.allocator), rs.end(repository.allocator));
#else
	    rs.push_back(r);
	    std::sort(rs.begin(), rs.end());
#endif
	  } else {
#if defined(NGT_SHARED_MEMORY_ALLOCATOR)
	    results.push_back(r, repository.allocator);
	    std::sort(results.begin(repository.allocator), results.end(repository.allocator));
#else
	    results.push_back(r);
	    std::sort(results.begin(), results.end());
#endif
	  }
	  output.pop_front();
	}

      } // for (;;)
    } // for (; retry == true; resSize = maxResSize) {

    int cnt = 0;
    for (size_t i = 0; i < delNodes.size(); i++) {
      if (delNodes[i].id != 0) {
	cnt++;
      }
    }
    if (cnt != 0) {
      for (size_t i = 0; i < delNodes.size(); i++) {
	if (delNodes[i].id != 0) {
	}
      }
    }
    threads.terminate();
  } catch (Exception &err) {
    threads.terminate();
    Exception e(err);
    throw e;
  }

  size_t delsize = osize - results.size();

  return delsize;
}

