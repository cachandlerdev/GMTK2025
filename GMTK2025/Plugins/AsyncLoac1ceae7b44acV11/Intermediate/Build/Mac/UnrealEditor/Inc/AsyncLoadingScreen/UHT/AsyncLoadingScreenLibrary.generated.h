// Copyright Epic Games, Inc. All Rights Reserved.
/*===========================================================================
	Generated code exported from UnrealHeaderTool.
	DO NOT modify this manually! Edit the corresponding .h files instead!
===========================================================================*/

// IWYU pragma: private, include "AsyncLoadingScreenLibrary.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"

PRAGMA_DISABLE_DEPRECATION_WARNINGS
#ifdef ASYNCLOADINGSCREEN_AsyncLoadingScreenLibrary_generated_h
#error "AsyncLoadingScreenLibrary.generated.h already included, missing '#pragma once' in AsyncLoadingScreenLibrary.h"
#endif
#define ASYNCLOADINGSCREEN_AsyncLoadingScreenLibrary_generated_h

#define FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	DECLARE_FUNCTION(execRemovePreloadedBackgroundImages); \
	DECLARE_FUNCTION(execPreloadBackgroundImages); \
	DECLARE_FUNCTION(execStopLoadingScreen); \
	DECLARE_FUNCTION(execGetIsEnableLoadingScreen); \
	DECLARE_FUNCTION(execSetEnableLoadingScreen); \
	DECLARE_FUNCTION(execSetDisplayMovieIndex); \
	DECLARE_FUNCTION(execSetDisplayTipTextIndex); \
	DECLARE_FUNCTION(execSetDisplayBackgroundIndex);


#define FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_INCLASS_NO_PURE_DECLS \
private: \
	static void StaticRegisterNativesUAsyncLoadingScreenLibrary(); \
	friend struct Z_Construct_UClass_UAsyncLoadingScreenLibrary_Statics; \
public: \
	DECLARE_CLASS(UAsyncLoadingScreenLibrary, UBlueprintFunctionLibrary, COMPILED_IN_FLAGS(0), CASTCLASS_None, TEXT("/Script/AsyncLoadingScreen"), NO_API) \
	DECLARE_SERIALIZER(UAsyncLoadingScreenLibrary)


#define FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_ENHANCED_CONSTRUCTORS \
	/** Standard constructor, called after all reflected properties have been initialized */ \
	NO_API UAsyncLoadingScreenLibrary(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get()); \
private: \
	/** Private move- and copy-constructors, should never be used */ \
	UAsyncLoadingScreenLibrary(UAsyncLoadingScreenLibrary&&); \
	UAsyncLoadingScreenLibrary(const UAsyncLoadingScreenLibrary&); \
public: \
	DECLARE_VTABLE_PTR_HELPER_CTOR(NO_API, UAsyncLoadingScreenLibrary); \
	DEFINE_VTABLE_PTR_HELPER_CTOR_CALLER(UAsyncLoadingScreenLibrary); \
	DEFINE_DEFAULT_OBJECT_INITIALIZER_CONSTRUCTOR_CALL(UAsyncLoadingScreenLibrary) \
	NO_API virtual ~UAsyncLoadingScreenLibrary();


#define FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_18_PROLOG
#define FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_GENERATED_BODY \
PRAGMA_DISABLE_DEPRECATION_WARNINGS \
public: \
	FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_RPC_WRAPPERS_NO_PURE_DECLS \
	FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_INCLASS_NO_PURE_DECLS \
	FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h_21_ENHANCED_CONSTRUCTORS \
private: \
PRAGMA_ENABLE_DEPRECATION_WARNINGS


template<> ASYNCLOADINGSCREEN_API UClass* StaticClass<class UAsyncLoadingScreenLibrary>();

#undef CURRENT_FILE_ID
#define CURRENT_FILE_ID FID_Build_U5M_Mac_Sync_LocalBuilds_PluginTemp_HostProject_Plugins_AsyncLoadingScreen_Source_AsyncLoadingScreen_Public_AsyncLoadingScreenLibrary_h


PRAGMA_ENABLE_DEPRECATION_WARNINGS
