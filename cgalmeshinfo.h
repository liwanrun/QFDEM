#ifndef CGALMESHINFO_H
#define CGALMESHINFO_H

#include <vector>

class VertInfo
{
public:
    // DefaultConstructible
    VertInfo()
    {
        this->id = -1;
        this->numCopy = 0;
        this->is_boundary = false;
        this->is_constrained = false;
    }

    // CopyConstructible
    VertInfo(const VertInfo& info)
    {
        this->id = info.id;
        this->copyIds = info.copyIds;
        this->numCopy = info.numCopy;
        this->is_boundary = info.is_boundary;
        this->is_constrained = info.is_constrained;
    }

    // CopyAssignable
    VertInfo &operator=(const VertInfo& info)
    {
        if(&info == this) return *this;

        this->id = info.id;
        this->copyIds = info.copyIds;
        this->numCopy = info.numCopy;
        this->is_boundary = info.is_boundary;
        this->is_constrained = info.is_constrained;

        return *this;
    }

public:
    size_t id;
    std::vector<size_t> copyIds;
    int numCopy;

    bool is_boundary;
    bool is_constrained;
};


class FaceInfo
{
public:
    // DefaultConstructible
    FaceInfo()
    {
        this->id = -1;
        this->verts[0] = -1;
        this->verts[1] = -1;
        this->verts[2] = -1;
        this->in_domain = false;

        this->gid = -1;
        this->fid = -1;
    }

    // CopyConstructibler
    FaceInfo(const FaceInfo& info)
    {
        this->id = info.id;
        this->verts[0] = info.verts[0];
        this->verts[1] = info.verts[1];
        this->verts[2] = info.verts[2];
        this->ranks = info.ranks;
        this->in_domain = info.in_domain;

        this->gid = info.gid;
        this->fid = info.fid;
    }

    //CopyAssignable
    FaceInfo &operator=(const FaceInfo& info)
    {
        if(&info == this) return *this;

        this->id = info.id;
        this->verts[0] = info.verts[0];
        this->verts[1] = info.verts[1];
        this->verts[2] = info.verts[2];
        this->ranks = info.ranks;
        this->in_domain = info.in_domain;

        this->gid = info.gid;
        this->fid = info.fid;

        return *this;
    }

public:
    size_t id;
    size_t verts[3];
    std::vector<int> ranks;

    bool in_domain;             // Only for Constrained Triangulation

    size_t gid;                 // grain_id
    size_t fid;                 // block_id
};

#endif // CGALMESHINFO_H
