#include "algorithms/tetra_hierarchy.h"


inline std::vector<glm::u64vec3> Diamond::ChildrenDiamonds() const
{
    switch (splitEdgeDirection) {
    case 0b00010101:
        return {{}, {}, {}};
    case 0b00000000:
        return {{}, {}};
    }
}   

void TetraHierarchy::DiamondCheckSplit(const Diamond& d, bool force) 
{
    if (!force && !ShouldSplitDiamond(d)) {
        return;
    }

    // ensure d is complete
    for (const vertex_t& p : ParentDiamonds(d)) {
        Diamond& pd = FindOrMakeDiamond(p);
        if (!pd.complete()) {
            DiamondCheckSplit(pd, true);
        }
    }    
    assert(d.complete());

    // actually split the diamond.
    // after this the diamond structure should still be sound.
    SplitDiamond(d);

    // recurse on children
    if (!force) {
        for (const vertex_t& c : ChildDiamonds(d)) {
            Diamond& cd = FindOrMakeDiamond(c);
            DiamondCheckSplit(cd);
        }
    }
}

void SplitDiamond(d)
{
    // split all of d's tetras.
}


