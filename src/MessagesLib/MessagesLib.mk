##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=MessagesLib
ConfigurationName      :=Debug
WorkspacePath          := "/home/sam1203/projects/OLIPlatform/OLIPlatform"
ProjectPath            := "/home/sam1203/projects/OLIPlatform/src/MessagesLib"
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
Preprocessors          :=
ObjectSwitch           :=-o 
ArchiveOutputSwitch    := 
PreprocessOnlySwitch   :=-E
ObjectsFileList        :="MessagesLib.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src/MessagesLib/src $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src/MessagesLib/include $(IncludeSwitch)/home/sam1203/projects/OLIPlatform/src/include 
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
Objects0=$(IntermediateDirectory)/src_MessageBase.cpp$(ObjectSuffix) $(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(ObjectSuffix) 



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
	@echo rebuilt > "/home/sam1203/projects/OLIPlatform/OLIPlatform/.build-debug/MessagesLib"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_MessageBase.cpp$(ObjectSuffix): src/MessageBase.cpp $(IntermediateDirectory)/src_MessageBase.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/MessagesLib/src/MessageBase.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_MessageBase.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_MessageBase.cpp$(DependSuffix): src/MessageBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_MessageBase.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_MessageBase.cpp$(DependSuffix) -MM "src/MessageBase.cpp"

$(IntermediateDirectory)/src_MessageBase.cpp$(PreprocessSuffix): src/MessageBase.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_MessageBase.cpp$(PreprocessSuffix) "src/MessageBase.cpp"

$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(ObjectSuffix): src/ClntMessages/ClntNewOrderRequestMsg.cpp $(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/MessagesLib/src/ClntMessages/ClntNewOrderRequestMsg.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(DependSuffix): src/ClntMessages/ClntNewOrderRequestMsg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(DependSuffix) -MM "src/ClntMessages/ClntNewOrderRequestMsg.cpp"

$(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(PreprocessSuffix): src/ClntMessages/ClntNewOrderRequestMsg.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/ClntMessages_ClntNewOrderRequestMsg.cpp$(PreprocessSuffix) "src/ClntMessages/ClntNewOrderRequestMsg.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


