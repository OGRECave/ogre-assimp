
#include "Importer.h"




bool Importer::CreateMesh(std::string MeshName, std::string filename)
{
	Ogre::MeshPtr meshPtr = Ogre::MeshManager::getSingletonPtr()->createManual(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	Ogre::SkeletonPtr mSkeleton = Ogre::SkeletonManager::getSingletonPtr()->create(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	Ogre::MaterialPtr materialPtr = Ogre::MaterialManager::getSingletonPtr()->create(MeshName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME, true);
	// Crappy globals
	// NB some of these are not directly used, but are required to
	//   instantiate the singletons used in the dlls
	///////////////////////////////////////////////////
	//////////////////options////////////////////////
	AssimpLoader::AssOptions opts;

	opts.quietMode = false;
	opts.logFile = "ass.log";
	opts.customAnimationName = "";
	opts.dest = "";
	opts.animationSpeedModifier = 1.0;
	opts.lodValue = 250000;
	opts.lodFixed = 0;
	opts.lodPercent = 20;
	opts.numLods = 0;
	opts.usePercent = true;
	// ignore program name
	char* source = 0;
	char* dest = 0;

	// Set up options
	Ogre::UnaryOptionList unOpt;
	Ogre::BinaryOptionList binOpt;

	unOpt["-q"] = false;
	unOpt["-3ds_ani_fix"] = false;
	unOpt["-3ds_dae_fix"] = false;
	unOpt["-shader"] = false;
	binOpt["-log"] = "ass.log";
	binOpt["-aniName"] = "";
	binOpt["-aniSpeedMod"] = 1.0f;
	binOpt["-l"] = "";
	binOpt["-v"] = "";
	binOpt["-s"] = "Distance";
	binOpt["-p"] = "";
	binOpt["-f"] = "";

	opts.params = AssimpLoader::LP_GENERATE_SINGLE_MESH;
	opts.source = filename;
	dest = ".";

	
	opts.dest = dest;
	///////////////////////////////////////////////////

	Ogre::LogManager* logMgr =Ogre::LogManager::getSingletonPtr();
	Ogre::Math* mth = 0;
	Ogre::LodStrategyManager *lodMgr = Ogre::LodStrategyManager::getSingletonPtr();
	Ogre::MaterialManager* matMgr = Ogre::MaterialManager::getSingletonPtr();
	Ogre::SkeletonManager* skelMgr = Ogre::SkeletonManager::getSingletonPtr();
	Ogre::MeshSerializer* meshSerializer = 0;
	//Ogre::XMLMeshSerializer* xmlMeshSerializer = 0;
	Ogre::SkeletonSerializer* skeletonSerializer = 0;
	//Ogre::XMLSkeletonSerializer* xmlSkeletonSerializer = 0;
	Ogre::HardwareBufferManager *bufferManager = 0;
	Ogre::MeshManager* meshMgr = Ogre::MeshManager::getSingletonPtr();
	Ogre::ResourceGroupManager* rgm = Ogre::ResourceGroupManager::getSingletonPtr();
	Ogre::ScriptCompilerManager* scmgr = Ogre::ScriptCompilerManager::getSingletonPtr();
	Ogre::ArchiveManager* archmgr = Ogre::ArchiveManager::getSingletonPtr();
	Ogre::FileSystemArchiveFactory* mfsarchf = 0;

    // Assume success
	bool retCode = true;

    try
    {
       

        // this log catches output from the parseArgs call and routes it to stdout only
        logMgr->createLog("Temporary log", false, true, true);

        // use the log specified by the cmdline params
        logMgr->setDefaultLog(logMgr->createLog(opts.logFile, false, true));
        // get rid of the temporary log as we use the new log now
        logMgr->destroyLog("Temporary log");


        mth = new Ogre::Math();
      

      
        matMgr->initialise();

        meshSerializer = new Ogre::MeshSerializer();
        //xmlMeshSerializer = new Ogre::XMLMeshSerializer();
        skeletonSerializer = new Ogre::SkeletonSerializer();
        //xmlSkeletonSerializer = new Ogre::XMLSkeletonSerializer();
       

			bufferManager = Ogre::HardwareBufferManager::getSingletonPtr();
			


        mfsarchf = OGRE_NEW Ogre::FileSystemArchiveFactory();
        Ogre::ArchiveManager::getSingleton().addArchiveFactory( mfsarchf );
	
 

        AssimpLoader loader;
		loader.convert(opts,meshPtr, mSkeleton,materialPtr, MeshName);

	

    }
    catch(Ogre::Exception& e)
    {
        std::cerr << "FATAL ERROR: " << e.getDescription() << std::endl;
        std::cerr << "ABORTING!" << std::endl;
        retCode =false;
    }


    return retCode;

};
 Ogre::MaterialPtr  Importer::get_material_ptr() {

	return this->materialPtr;
 };