#ifndef MULTICASCADETREE_H
#define MULTICASCADETREE_H

#include <vector>

class MultiCascadeNode
{
public:
    MultiCascadeNode();
    void UpdateProbability();

public:
    int m_row;
    int m_col;
    int m_depth;
    int m_degree;
    double m_chance;

    MultiCascadeNode *parent;
    MultiCascadeNode *children[4];
};

class MultiCascadeTree
{
public:
    MultiCascadeTree(int height = 7, double dim = 1.5);

    void ConstructMultiCascadeTree();

    void UpdateProbability();

    std::vector<double> CreateProbabilitySequence();

private:
    std::vector<double> SamplePartitionOfUnityChances();

    MultiCascadeNode *ConstructMultiCascadeTreePrivate(int height);

private:
    int m_height;
    double m_fdim;
    std::vector<double> chances;

    MultiCascadeNode *root;
};

#endif // MULTICASCADETREE_H
