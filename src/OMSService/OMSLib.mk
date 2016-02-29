##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=OMSLib
ConfigurationName      :=Debug
WorkspacePath          := "/home/sam1203/projects/OLIPlatform/OLIPlatform"
ProjectPath            := "/home/sam1203/projects/OLIPlatform/src/OMSService"
IntermediateDirectory  :=./Debug
OutDir                 := $(IntermediateDirectory)
CurrentFileName        :=
CurrentFilePath        :=
CurrentFileFullPath    :=
User                   :=
Date                   :=29/02/16
CodeLitePath           :="/home/sam1203/.codelite"
LinkerName             :=/usr/bin/g++
SharedObjectLinkerName :=/usr/bin/g++ -shared -fPIC
ObjectSuffix           :=.o
DependSuffix           :=.o.d
PreprocessSuffix       :=.i
DebugSwitch            :=-g 
IncludeSwitch          :=-I
LibrarySwitch          :=-l
OutputSwitch           :=-o 
LibraryPathSwitch      :=-L
PreprocessorSwitch     :=-D
SourceSwitch           :=-c 
OutputFile             :=$(IntermediateDirectory)/lib$(ProjectName).a
Preprocessors          :=$(PreprocessorSwitch)FUSION_MAX_VECTOR_SIZE=40 $(PreprocessorSwitch)BOOST_MPL_CFG_NO_PREPROCESSED_HEADERS $(PreprocessorSwitch)BOOST_MPL_LIMIT_VECTOR_SIZE=40 $(PreprocessorSwitch)BOOST_MPL_LIMIT_MAP_SIZE=30 
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="OMSLib.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)/home/sam1203/DevTools/boost_1_58_0 $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src/OMSService/src $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src/include $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src 
IncludePCH             := 
RcIncludePath          := 
Libs                   := 
ArLibs                 :=  
LibPath                := $(LibraryPathSwitch). 

##
## Common variables
## AR, CXX, CC, AS, CXXFLAGS and CFLAGS can be overriden using an environment variables
##
AR       := /usr/bin/ar rcu
CXX      := /usr/bin/g++
CC       := /usr/bin/gcc
CXXFLAGS := -std=c++14 -std=c++11 -g $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
OLISolution:=/home/sam1203/projects/OLIPlatform/src
Objects0=$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_OrderExpirationController.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_OMSDataCache.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_OMSService.cpp$(ObjectSuffix) $(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(ObjectSuffix) $(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(ObjectSuffix) 



Objects=$(Objects0) 

##
## Main Build Targets 
##
.PHONY: all clean PreBuild PrePreBuild PostBuild MakeIntermediateDirs
all: $(IntermediateDirectory) $(OutputFile)

$(OutputFile): $(Objects)
	@$(MakeDirCommand) $(@D)
	@echo "" > $(IntermediateDirectory)/.d
	@echo $(Objects0)  > $(ObjectsFileList)
	$(AR) $(ArchiveOutputSwitch)$(OutputFile) @$(ObjectsFileList) $(ArLibs)
	@$(MakeDirCommand) "/home/sam1203/projects/OLIPlatform/OLIPlatform/.build-debug"
	@echo rebuilt > "/home/sam1203/projects/OLIPlatform/OLIPlatform/.build-debug/OMSLib"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(ObjectSuffix): src/OMSMessageDispatcher.cpp $(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/OMSMessageDispatcher.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(DependSuffix): src/OMSMessageDispatcher.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(DependSuffix) -MM "src/OMSMessageDispatcher.cpp"

$(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(PreprocessSuffix): src/OMSMessageDispatcher.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_OMSMessageDispatcher.cpp$(PreprocessSuffix) "src/OMSMessageDispatcher.cpp"

$(IntermediateDirectory)/src_OrderExpirationController.cpp$(ObjectSuffix): src/OrderExpirationController.cpp $(IntermediateDirectory)/src_OrderExpirationController.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/OrderExpirationController.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_OrderExpirationController.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_OrderExpirationController.cpp$(DependSuffix): src/OrderExpirationController.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_OrderExpirationController.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_OrderExpirationController.cpp$(DependSuffix) -MM "src/OrderExpirationController.cpp"

$(IntermediateDirectory)/src_OrderExpirationController.cpp$(PreprocessSuffix): src/OrderExpirationController.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_OrderExpirationController.cpp$(PreprocessSuffix) "src/OrderExpirationController.cpp"

$(IntermediateDirectory)/src_OMSDataCache.cpp$(ObjectSuffix): src/OMSDataCache.cpp $(IntermediateDirectory)/src_OMSDataCache.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/OMSDataCache.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_OMSDataCache.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_OMSDataCache.cpp$(DependSuffix): src/OMSDataCache.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_OMSDataCache.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_OMSDataCache.cpp$(DependSuffix) -MM "src/OMSDataCache.cpp"

$(IntermediateDirectory)/src_OMSDataCache.cpp$(PreprocessSuffix): src/OMSDataCache.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_OMSDataCache.cpp$(PreprocessSuffix) "src/OMSDataCache.cpp"

$(IntermediateDirectory)/src_OMSService.cpp$(ObjectSuffix): src/OMSService.cpp $(IntermediateDirectory)/src_OMSService.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/OMSService.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_OMSService.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_OMSService.cpp$(DependSuffix): src/OMSService.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_OMSService.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_OMSService.cpp$(DependSuffix) -MM "src/OMSService.cpp"

$(IntermediateDirectory)/src_OMSService.cpp$(PreprocessSuffix): src/OMSService.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_OMSService.cpp$(PreprocessSuffix) "src/OMSService.cpp"

$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(ObjectSuffix): src/StateMachine/OMSStateMachine.cpp $(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/StateMachine/OMSStateMachine.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(DependSuffix): src/StateMachine/OMSStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(DependSuffix) -MM "src/StateMachine/OMSStateMachine.cpp"

$(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(PreprocessSuffix): src/StateMachine/OMSStateMachine.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StateMachine_OMSStateMachine.cpp$(PreprocessSuffix) "src/StateMachine/OMSStateMachine.cpp"

$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(ObjectSuffix): src/StateMachine/OMSStateMachineImpl.cpp $(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/OMSService/src/StateMachine/OMSStateMachineImpl.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(DependSuffix): src/StateMachine/OMSStateMachineImpl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(DependSuffix) -MM "src/StateMachine/OMSStateMachineImpl.cpp"

$(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(PreprocessSuffix): src/StateMachine/OMSStateMachineImpl.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/StateMachine_OMSStateMachineImpl.cpp$(PreprocessSuffix) "src/StateMachine/OMSStateMachineImpl.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


