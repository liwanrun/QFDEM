#include "multicascadetree.h"

#include <queue>
#include <random>
#include <QDebug>

MultiCascadeNode::MultiCascadeNode()
    : m_row(0), m_col(0), m_depth(0), m_degree(4), m_chance(1.0)
{
    parent = nullptr;
    children[0] = nullptr;
    children[1] = nullptr;
    children[2] = nullptr;
    children[3] = nullptr;
}

void MultiCascadeNode::UpdateProbability()
{
    if(this->parent)
    {
        m_chance *= this->parent->m_chance;
    }

    if(this->m_degree)
    {
        for(int i = 0; i < 4; i++)
        {
            int r = (i >> 1); int c = (i & 1);
            this->children[i]->m_row = 2 * m_row + r;
            this->children[i]->m_col = 2 * m_col + c;
            this->children[i]->UpdateProbability();
        }
    }
}

MultiCascadeTree::MultiCascadeTree(int height, double dim)
    : m_height(height), m_fdim(dim)
{
    this->root = nullptr;

    if(height) this->ConstructMultiCascadeTree();
}

void MultiCascadeTree::ConstructMultiCascadeTree()
{
    this->chances = this->SamplePartitionOfUnityChances();
    this->root = this->ConstructMultiCascadeTreePrivate(m_height);
}

void MultiCascadeTree::UpdateProbability()
{
    this->root->UpdateProbability();
}

std::vector<double> MultiCascadeTree::CreateProbabilitySequence()
{
    size_t ndivs = static_cast<size_t>(std::pow(2, this->m_height - 1));
    std::vector<double> sequence(ndivs * ndivs + 1);

    //Traverse in level
    std::queue<MultiCascadeNode *> nodeQueue;
    nodeQueue.emplace(this->root);
    while(!nodeQueue.empty())
    {
        MultiCascadeNode* node = nodeQueue.front();
        nodeQueue.pop();

        if(node->m_degree)
        {
            for(int i = 0; i < node->m_degree; i++)
            {
                nodeQueue.emplace(node->children[i]);
            }
        }
        else
        {
            size_t index = static_cast<size_t>(node->m_row * ndivs + node->m_col + 1);
            sequence[index] = node->m_chance;
        }
    }

    qDebug()<<std::accumulate(sequence.begin(),sequence.end(),0.0);
    //accumulate probability matrix
    for(size_t i = 1; i < sequence.size(); i++)
    {
        sequence[i] = sequence[i] + sequence[i-1];
    }

    return sequence;
}

std::vector<double> MultiCascadeTree::SamplePartitionOfUnityChances()
{
    std::vector<double> chances(4);
    if(2.0 == this->m_fdim)
    {
        chances[0] = 0.25;
        chances[1] = 0.25;
        chances[2] = 0.25;
        chances[3] = 0.25;

        return chances;
    }

    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<double> distribution;

    double alpha = std::pow(0.5, this->m_fdim);
    double p0, p1, p2, p3, delta;
    do
    {
        p0 = distribution(generator);
        p1 = distribution(generator);
        p2 = 0.0;
        p3 = 0.0;

        delta = -1.0 + 2 * alpha + 2 * p0 - 3 * std::pow(p0, 2) + 2 * p1 - 2 * p0 * p1 - 3 * std::pow(p1, 2);
        if(delta > 0.0)
        {
            p2 = (1 - p0 - p1 - std::sqrt(delta)) / 2.0;
            p3 = (1 - p0 - p1 + std::sqrt(delta)) / 2.0;
        }

    }while(!((p2 > 0.0) && (p3 > 0.0)));

    chances[0] = p0;
    chances[1] = p1;
    chances[2] = p2;
    chances[3] = p3;

//    qDebug() << "p0 = " << p0 << ","
//             << "p1 = " << p1 << ","
//             << "p2 = " << p2 << ","
//             << "p3 = " << p3 ;
//    qDebug() << "p0^2 + p1^2 + p2^2 + p3^2 = " << p0*p0 + p1*p1 + p2*p2 +p3*p3;

    return chances;
}

MultiCascadeNode *MultiCascadeTree::ConstructMultiCascadeTreePrivate(int height)
{
    if(height == 0) { return nullptr; }

    std::random_device rd;
    std::vector<double> chances(this->chances);
    //std::vector<double> vector(this->PartitionOfUnity());
    std::shuffle(chances.begin(), chances.end(), rd);

    MultiCascadeNode *node = new MultiCascadeNode();
    node->m_depth = this->m_height - height;
    for(size_t i = 0; i < node->m_degree; i++)
    {
        MultiCascadeNode *child = ConstructMultiCascadeTreePrivate(height - 1);
        node->children[i] = child;
        if(child)
        {
            child->m_chance = chances[i];
            child->parent = node;
        }
        else
        {
            node->m_degree = 0;
        }
    }

    return node;
}
