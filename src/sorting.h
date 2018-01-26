#ifndef SRC_SORTING_H_
#define SRC_SORTING_H_


template<typename Sequence, typename BinaryPredicate>
struct IndexCompareT
{
	IndexCompareT(const Sequence& seq, const BinaryPredicate comp) :
			seq_(seq), comp_(comp)
	{
	}
	bool operator()(const size_t a, const size_t b) const
	{
		return comp_(seq_[a], seq_[b]);
	}
	const Sequence seq_;
	const BinaryPredicate comp_;
};

template<typename Sequence, typename BinaryPredicate>
IndexCompareT<Sequence, BinaryPredicate> IndexCompare(const Sequence& seq,
		const BinaryPredicate comp)
{
	return IndexCompareT<Sequence, BinaryPredicate>(seq, comp);
}

template<typename Sequence, typename BinaryPredicate>
std::vector<int> ArgSort(const Sequence& seq, BinaryPredicate func)
{
	std::vector<int> index(seq.size());
	for (int i = 0; i < (int) index.size(); i++)
	{
		index[i] = i;
	}
	std::sort(index.begin(), index.end(), IndexCompare(seq, func));

	return index;
}


#endif /* SRC_SORTING_H_ */
