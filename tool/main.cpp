/*
-----------------------------------------------------------------------------
This source file is part of
                                    _
  ___   __ _ _ __ ___  __ _ ___ ___(_)_ __ ___  _ __
 / _ \ / _` | '__/ _ \/ _` / __/ __| | '_ ` _ \| '_ \
| (_) | (_| | | |  __/ (_| \__ \__ \ | | | | | | |_) |
 \___/ \__, |_|  \___|\__,_|___/___/_|_| |_| |_| .__/
       |___/                                   |_|

For the latest info, see http://code.google.com/p/ogreassimp/

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
#include <iostream>
#include <sys/stat.h>

#include <Ogre.h>
#include <OgreString.h>
#include <OgreMeshSerializer.h>
#include <OgreSkeletonSerializer.h>
#include <OgreDefaultHardwareBufferManager.h>
#include <OgreProgressiveMesh.h>
#include <OgreScriptCompiler.h>
#include <OgreFileSystem.h>

#include "AssimpLoader.h"

struct AssOptions
{
    Ogre::String source;
    Ogre::String dest;
    bool quietMode;
    Ogre::String logFile;
    Ogre::String customAnimationName;
    int params;
    Ogre::Real animationSpeed;
};

void help(void)
{
    // Print help message
    std::cout << std::endl << "OgreAssimpConverter: Converts data from model formats supported by Assimp" << std::endl;
    std::cout << "to OGRE binary formats (mesh and skeleton) and material script." << std::endl;
    std::cout << std::endl << "Usage: OgreAssimpConverter [options] sourcefile [destination] " << std::endl;
    std::cout << std::endl << "Available options:" << std::endl;
    std::cout << "-q            = Quiet mode, less output" << std::endl;
    std::cout << "-log filename = name of the log file (default: 'ass.log')" << std::endl;
    std::cout << "-aniSpeed     = Factor to scale the animation speed - defaults to '1.0f'" << std::endl;
    std::cout << "-3ds_ani_fix  = Fix for the fact that 3ds max exports the animation over a" << std::endl;
    std::cout << "                longer time frame than the animation actually plays for" << std::endl;
    std::cout << "-3ds_dae_fix  = When 3ds max exports as DAE it gets some of the transforms wrong, get around this" << std::endl;
    std::cout << "                by using this option and a prior run with of the model exported as ASE" << std::endl;
    std::cout << "sourcefile    = name of file to convert" << std::endl;
    std::cout << "destination   = optional name of directory to write to. If you don't" << std::endl;
    std::cout << "                 specify this the converter will use the same directory as the sourcefile."  << std::endl;
    std::cout << std::endl;
}

AssOptions parseArgs(int numArgs, char **args)
{
    AssOptions opts;
    opts.quietMode = false;
    opts.logFile = "ass.log";
    opts.customAnimationName = "";
    opts.dest = "";

    // ignore program name
    char* source = 0;
    char* dest = 0;

    // Set up options
    Ogre::UnaryOptionList unOpt;
    Ogre::BinaryOptionList binOpt;

    unOpt["-q"] = false;
    unOpt["-3ds_ani_fix"] = false;
    unOpt["-3ds_dae_fix"] = false;
    binOpt["-log"] = "ass.log";
    binOpt["-aniName"] = "";
    binOpt["-aniSpeed"] = 1.0f;

    int startIndex = Ogre::findCommandLineOpts(numArgs, args, unOpt, binOpt);
    Ogre::UnaryOptionList::iterator ui;
    Ogre::BinaryOptionList::iterator bi;

    ui = unOpt.find("-q");
    if (ui->second)
    {
        opts.quietMode = true;
    }

    opts.params = (AssimpLoader::LP_GENERATE_SINGLE_MESH | AssimpLoader::LP_GENERATE_MATERIALS_AS_CODE);

    ui = unOpt.find("-3ds_ani_fix");
    if (ui->second)
    {
        opts.params |= AssimpLoader::LP_CUT_ANIMATION_WHERE_NO_FURTHER_CHANGE;
    }
    ui = unOpt.find("-3ds_dae_fix");
    if (ui->second)
    {
        opts.params |= AssimpLoader::LP_USE_LAST_RUN_NODE_DERIVED_TRANSFORMS;
    }

    bi = binOpt.find("-log");
    if (!bi->second.empty())
    {
        opts.logFile = bi->second;
    }

    bi = binOpt.find("-aniSpeed");
    if (!bi->second.empty())
    {
        opts.animationSpeed = Ogre::StringConverter::parseReal(bi->second);
    }

    bi = binOpt.find("-aniName");
    if (!bi->second.empty())
    {
        opts.customAnimationName = bi->second;
    }

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
    opts.source = source;

    if (dest)
    {
        opts.dest = dest;
    }

    if (!opts.quietMode)
    {
        std::cout << std::endl;
        std::cout << "-- OPTIONS --" << std::endl;

        std::cout << "source file      = " << opts.source << std::endl;
        std::cout << "destination      = " << opts.dest << std::endl;
        std::cout << "animation speed  = " << opts.animationSpeed << std::endl;
        std::cout << "log file         = " << opts.logFile << std::endl;

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
Ogre::ScriptCompilerManager* scmgr = 0;
Ogre::ArchiveManager* archmgr = 0;
Ogre::FileSystemArchiveFactory* mfsarchf = 0;

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
        scmgr = new Ogre::ScriptCompilerManager();
        archmgr = new Ogre::ArchiveManager();
        mfsarchf = new Ogre::FileSystemArchiveFactory();
        Ogre::ArchiveManager::getSingleton().addArchiveFactory( mfsarchf );

        if(opts.quietMode)
            opts.params |= AssimpLoader::LP_QUIET_MODE;

        AssimpLoader loader;
        loader.convert(opts.source, opts.customAnimationName, opts.params, opts.dest, opts.animationSpeed);

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
    //delete meshMgr; FIX this!!
    delete bufferManager;
    delete scmgr;
    delete archmgr;
    delete mfsarchf;
    delete lodMgr;
    delete mth;
    delete rgm;
    delete logMgr;

    return retCode;

}

