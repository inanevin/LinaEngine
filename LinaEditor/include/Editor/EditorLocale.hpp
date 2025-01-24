/*
This file is a part of: Lina Engine
https://github.com/inanevin/LinaEngine

Author: Inan Evin
http://www.inanevin.com

Copyright (c) [2018-] [Inan Evin]

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#pragma once
#include "Common/Data/String.hpp"
#include "Common/Data/Map.hpp"

namespace Lina::Editor
{

	enum class LocaleStr
	{
		Save,
		Load,
		Yes,
		No,
		Hex,
		File,
		Edit,
		View,
		Panels,
		About,
		Create,
		Delete,
		Duplicate,
		Open,
		Cancel,
		Name,
		Path,
		Location,
		Select,
		CreatingProject,
		ProjectSelect,
		CreateNewProject,
		OpenExistingProject,
		SelectDirectory,
		CreateANewProject,
		CreateANewWorld,
		CreateANewShader,
		SelectProjectFile,
		DirectoryNotFound,
		FileNotFound,
		NameIsNotValid,
		Empty,
		ExitEditor,
		UnfinishedWorkTitle,
		UnfinishedWorkDesc,
		LinaProjectFile,
		LinaWorldFile,
		NoProject,
		NoProjectFound,
		NewProject,
		LoadProject,
		SaveProject,
		NewWorld,
		LoadWorld,
		SaveWorld,
		SaveWorldAs,
		Exit,
		Github,
		Discord,
		Website,
		More,
		Entities,
		Details,
		Resources,
		World,
		Performance,
		ResetLayout,
		Search,
		Items,
		Selected,
		NothingInDirectory,
		Memory,
		PeakMemory,
		FrameTime,
		DrawCalls,
		GeneratingThumbnails,
		ContentSorting,
		Alphabetical,
		TypeBased,
		Favourites,
		Folder,
		Material,
		PhysicsMaterial,
		Sampler,
		Shader,
		OpaqueSurfaceShader,
		SkyShader,
		TransparentSurfaceShader,
		PostProcessShader,
		AddToFavourites,
		RemoveFromFavourites,
		RemoveAll,
		AreYouSureYouWantToDeleteI,
		AreYouSureYouWantToDeleteII,
		Minimize,
		Maximize,
		MaximizeRestore,
		Close,
		Rename,
		WidgetEditor,
		Import,
		ReImport,
		ReimportChangedFiles,
		Revert,
		Reverting,
		Reimporting,
		New,
		GUIWidget,
		SelectWidget,
		SaveChanges,
		SaveChangesDesc,
		SelectColor,
		StartColor,
		EndColor,
		ThemeColor,
		Both,
		Export,
		Refresh,
		ImportingResources,
		ResourceNotFound,
		NoWorldInstalled,
		GenerateSDF,
		General,
		Resource,
		Working,
		WorkInProgressInAnotherWindow,
		LoadingProjectData,
		CopyingGameProjectFiles,
		VerifyingProjectResources,
		CreatingCoreResources,
		Saving,
		LoadingEngine,
		LoadingSettings,
		LoadingCoreResources,
		LoadingWorld,
		GeneratingAtlases,
		ApplyingChanges,
		FailedReimportingResource,
		ReimportedResource,
		ReimportShader,
		AutoReimport,
		Log,
		EnterConsoleCommand,
		LogLevels,
		Info,
		Warning,
		Error,
		Trace,
		ChangeSourceAsset,
		MultipleItems,
		LinaAssets,
		Filter,
		None,
		Profiling,
		ApplicationTime,
		ID,
		Type,
		Preview,
		Spaces,
		Meta,
		SizeHW,
		CPUUtilization,
		Vertices,
		Indices,
		Delta,
		SmoothedDelta,
		Tris,
		DPIScale,
		PreviewAnimation,
		Move,
		Position,
		Rotation,
		Scale,
		Rotate,
		VisibilityOn,
		VisibilityOff,
		LocalityWorld,
		LocalityLocal,
		NoSnapping,
		GridSnapping,
		Parenting,
		SnapOptions,
		WorldOptions,
		CameraOptions,
		PhysicsOptions,
		ResetTransform,
		Physics,
		NoEntityDetails,
		Play,
		PlayPhysics,
		Display,
		PointLight,
		SpotLight,
		DirectionalLight,
		AmbientLight,
		LoadGamePlugin,
		LoadGamePluginSuccess,
		LoadGamePluginFail,
		Packaging,
		BuildPackage,
		ProjectSettings,
		ChooseWhereToPackage,
		Entity,
		AddComponent,
		Widget,
		Text,
		Icon,
		DirectionalLayout,
		Button,
		GameProjectOutDoesntExist,
		SuccessfullyPackagedProject,
		Parameters,
		SavedWorld,
		InputSettings,
		EditorSettings,
	};

	class Locale
	{
	public:
		static const String& GetStr(LocaleStr str);
		static String		 GetStrUnicode(LocaleStr str);

		static String GetRandomFact();

	private:
		static HashMap<LocaleStr, String> s_englishMap;
		static Vector<String>			  s_randomFacts;
	};

} // namespace Lina::Editor
