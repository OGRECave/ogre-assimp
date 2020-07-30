/*
-----------------------------------------------------------------------------
This source file is part of
                                    _
  ___   __ _ _ __ ___  __ _ ___ ___(_)_ __ ___  _ __
 / _ \ / _` | '__/ _ \/ _` / __/ __| | '_ ` _ \| '_ \
| (_) | (_| | | |  __/ (_| \__ \__ \ | | | | | | |_) |
 \___/ \__, |_|  \___|\__,_|___/___/_|_| |_| |_| .__/
       |___/                                   |_|

For the latest info, see https://bitbucket.org/jacmoe/ogreassimp

Copyright (c) 2011 Jacob 'jacmoe' Moen

Licensed under the MIT license:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef __AssimpLoader_h__
#define __AssimpLoader_h__

#include <string>
#include <OgreMesh.h>
//#include <assimp/assimp.hpp>
//#include <assimp/aiScene.h>
//#include <assimp/aiPostProcess.h>
#include "assimp/Importer.hpp"
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <map>

class AssimpLoader
{
public:
    struct AssOptions
    {
        Ogre::String source;
        Ogre::String dest;
        bool quietMode;
        Ogre::String logFile;
        Ogre::String customAnimationName;
        int params;
        float animationSpeedModifier;
        float maxEdgeAngle;

        AssOptions()
        {
            quietMode = false;
            logFile = "ass.log";
            params = 0;
            animationSpeedModifier = 1.0;
            maxEdgeAngle = 30;
        };
    };

    enum LoaderParams
    {
        // 3ds max exports the animation over a longer time frame than the animation actually plays for
        // this is a fix for that
        LP_CUT_ANIMATION_WHERE_NO_FURTHER_CHANGE = 1<<0,

        // when 3ds max exports as DAE it gets some of the transforms wrong, get around this by using
        // this option and a prior run with of the model exported as ASE
        LP_USE_LAST_RUN_NODE_DERIVED_TRANSFORMS = 1<<1,

        // Quiet mode - don't output anything
        LP_QUIET_MODE = 1<<2
    };

    AssimpLoader();
    virtual ~AssimpLoader();

    bool convert(const AssOptions options, Ogre::MeshPtr *meshPtr = NULL, Ogre::SkeletonPtr *skeletonPtr = NULL);

    const Ogre::String& getBasename(){ return mBasename; }

private:
    bool createSubMesh(const Ogre::String& name, int index, const aiNode* pNode, const aiMesh *mesh, const aiMaterial* mat, Ogre::MeshPtr mMesh, Ogre::AxisAlignedBox& mAAB, const Ogre::String& mDir);
    Ogre::MaterialPtr createMaterial(int index, const aiMaterial* mat, const Ogre::String& mDir);
    void grabNodeNamesFromNode(const aiScene* mScene,  const aiNode* pNode);
    void grabBoneNamesFromNode(const aiScene* mScene,  const aiNode* pNode);
    void computeNodesDerivedTransform(const aiScene* mScene,  const aiNode *pNode, const aiMatrix4x4 accTransform);
    void createBonesFromNode(const aiScene* mScene,  const aiNode* pNode);
    void createBoneHiearchy(const aiScene* mScene,  const aiNode *pNode);
    void loadDataFromNode(const aiScene* mScene,  const aiNode *pNode, const Ogre::String& mDir);
    void markAllChildNodesAsNeeded(const aiNode *pNode);
    void flagNodeAsNeeded(const char* name);
    bool isNodeNeeded(const char* name);
    void parseAnimation (const aiScene* mScene, int index, aiAnimation* anim);
    typedef std::map<Ogre::String, bool> boneMapType;
    boneMapType boneMap;
    //aiNode* mSkeletonRootNode;
    int mLoaderParams;
    Ogre::String mBasename;
    Ogre::String mPath;
    Ogre::String mCustomAnimationName;

    typedef std::map<Ogre::String, const aiNode*> BoneNodeMap;
    BoneNodeMap mBoneNodesByName;

    typedef std::map<Ogre::String, const aiBone*> BoneMap;
    BoneMap mBonesByName;

    typedef std::map<Ogre::String, aiMatrix4x4> NodeTransformMap;
    NodeTransformMap mNodeDerivedTransformByName;

    Ogre::MeshPtr mMesh;
    Ogre::SkeletonPtr mSkeleton;

    static int msBoneCount;

    bool mQuietMode;
    Ogre::Real mTicksPerSecond;
    Ogre::Real mAnimationSpeedModifier;
};

#endif // __AssimpLoader_h__
