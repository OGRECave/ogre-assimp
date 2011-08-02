/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2009 Torus Knot Software Ltd

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
#include <iostream>
#include <sys/stat.h>

#include <Ogre.h>
#include <OgreString.h>
#include <OgreMeshSerializer.h>
#include <OgreSkeletonSerializer.h>
#include <OgreDefaultHardwareBufferManager.h>
#include <OgreProgressiveMesh.h>

#include "AssimpLoader.h"

struct AssOptions
{
    Ogre::String source;
    Ogre::String dest;
    Ogre::String sourceExt;
    Ogre::String destExt;
    Ogre::String logFile;
};

void help(void)
{
    // Print help message
    std::cout << std::endl << "OgreAssimpConverter: Converts data between XML and OGRE binary formats." << std::endl;
    std::cout << "Provided for OGRE by Steve Streeting" << std::endl << std::endl;
    std::cout << "Usage: OgreXMLConverter [options] sourcefile [destfile] " << std::endl;
    std::cout << std::endl << "Available options:" << std::endl;
    std::cout << "sourcefile     = name of file to convert" << std::endl;
    std::cout << "destfile       = optional name of file to write to. If you don't" << std::endl;
    std::cout << "                 specify this OGRE works it out through the extension " << std::endl;
    std::cout << "                 and the XML contents if the source is XML. For example" << std::endl;
    std::cout << "                 test.mesh becomes test.xml, test.xml becomes test.mesh " << std::endl;
    std::cout << "                 if the XML document root is <mesh> etc."  << std::endl;

    std::cout << std::endl;
}

AssOptions parseArgs(int numArgs, char **args)
{
    AssOptions opts;
    opts.logFile = "ass.log";

    // ignore program name
    char* source = 0;
    char* dest = 0;

    int startIndex = 1;

    // Source / dest
    if (numArgs > startIndex)
        source = args[startIndex];
    if (numArgs > startIndex+1)
        dest = args[startIndex+1];
    if (numArgs > startIndex+2) {
        std::cout << "Too many command-line arguments supplied - abort. " << std::endl;
        help();
        exit(1);
    }

    if (!source)
    {
        std::cout << "Missing source file - abort. " << std::endl;
        help();
        exit(1);
    }
    // Work out what kind of conversion this is
    opts.source = source;
    Ogre::vector<Ogre::String>::type srcparts = Ogre::StringUtil::split(opts.source, ".");
    Ogre::String& ext = srcparts.back();
    Ogre::StringUtil::toLowerCase(ext);
    opts.sourceExt = ext;

    if (!dest)
    {
        if (opts.sourceExt == "xml")
        {
            // dest is source minus .xml
            opts.dest = opts.source.substr(0, opts.source.size() - 4);
        }
        else
        {
            // dest is source + .xml
            opts.dest = opts.source;
            opts.dest.append(".xml");
        }

    }
    else
    {
        opts.dest = dest;
    }
    Ogre::vector<Ogre::String>::type dstparts = Ogre::StringUtil::split(opts.dest, ".");
    ext = dstparts.back();
    Ogre::StringUtil::toLowerCase(ext);
    opts.destExt = ext;

    if (true)
    {
        std::cout << std::endl;
        std::cout << "-- OPTIONS --" << std::endl;
        std::cout << "source file      = " << opts.source << std::endl;
        std::cout << "destination file = " << opts.dest << std::endl;

        std::cout << "-- END OPTIONS --" << std::endl;
        std::cout << std::endl;
    }

    return opts;
}

// Crappy globals
// NB some of these are not directly used, but are required to
//   instantiate the singletons used in the dlls
Ogre::LogManager* logMgr = 0;
Ogre::Math* mth = 0;
Ogre::LodStrategyManager *lodMgr = 0;
Ogre::MaterialManager* matMgr = 0;
Ogre::SkeletonManager* skelMgr = 0;
Ogre::MeshSerializer* meshSerializer = 0;
//Ogre::XMLMeshSerializer* xmlMeshSerializer = 0;
Ogre::SkeletonSerializer* skeletonSerializer = 0;
//Ogre::XMLSkeletonSerializer* xmlSkeletonSerializer = 0;
Ogre::DefaultHardwareBufferManager *bufferManager = 0;
Ogre::MeshManager* meshMgr = 0;
Ogre::ResourceGroupManager* rgm = 0;

int main(int numargs, char** args)
{
    if (numargs < 2)
    {
        help();
        return -1;
    }

    // Assume success
	int retCode = 0;

    try
    {
        logMgr = new Ogre::LogManager();

        // this log catches output from the parseArgs call and routes it to stdout only
        logMgr->createLog("Temporary log", false, true, true);

        AssOptions opts = parseArgs(numargs, args);
        // use the log specified by the cmdline params
        logMgr->setDefaultLog(logMgr->createLog(opts.logFile, false, true));
        // get rid of the temporary log as we use the new log now
        logMgr->destroyLog("Temporary log");

        rgm = new Ogre::ResourceGroupManager();
        mth = new Ogre::Math();
        lodMgr = new Ogre::LodStrategyManager();
        meshMgr = new Ogre::MeshManager();
        matMgr = new Ogre::MaterialManager();
        matMgr->initialise();
        skelMgr = new Ogre::SkeletonManager();
        meshSerializer = new Ogre::MeshSerializer();
        //xmlMeshSerializer = new Ogre::XMLMeshSerializer();
        skeletonSerializer = new Ogre::SkeletonSerializer();
        //xmlSkeletonSerializer = new Ogre::XMLSkeletonSerializer();
        bufferManager = new Ogre::DefaultHardwareBufferManager(); // needed because we don't have a rendersystem

    }
    catch(Ogre::Exception& e)
    {
        std::cerr << "FATAL ERROR: " << e.getDescription() << std::endl;
        std::cerr << "ABORTING!" << std::endl;
        retCode = 1;
    }

    //delete xmlSkeletonSerializer;
    delete skeletonSerializer;
    //delete xmlMeshSerializer;
    delete meshSerializer;
    delete skelMgr;
    delete matMgr;
    delete meshMgr;
    delete bufferManager;
    delete lodMgr;
    delete mth;
    delete rgm;
    delete logMgr;

    return retCode;

}

