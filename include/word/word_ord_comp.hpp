#ifndef WORD_ORD_COMP_HPP
#define WORD_ORD_COMP_HPP

namespace ga::w {

////////////////////////////////////////////////////////////////////////////////
/// \brief The WordOrdComp class
///
template <class WordT>
class WordOrdComp {
public:
    bool operator()(const WordT& w0, const WordT& w1) const;
};

////////////////////////////////////////////////////////////////////////////////
//----------------------------------------------------------------------------//
template <class WordT>
bool WordOrdComp<WordT>::operator()(const WordT& w0, const WordT& w1) const {
    return WordT::ord(w0) < WordT::ord(w1);
}

}

#endif // WORD_ORD_COMP_HPP
