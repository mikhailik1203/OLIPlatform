##
## Auto Generated makefile by CodeLite IDE
## any manual changes will be erased      
##
## Debug
ProjectName            :=TransportLayerLib
ConfigurationName      :=Debug
WorkspacePath          := "/home/sam1203/projects/OLIPlatform/OLIPlatform"
ProjectPath            := "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib"
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
ObjectsFileList        :="TransportLayerLib.txt"
PCHCompileFlags        :=
MakeDirCommand         :=mkdir -p
LinkOptions            :=  
IncludePath            :=  $(IncludeSwitch). $(IncludeSwitch). 
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
CXXFLAGS :=  -g -std=c++14 -std=c++11 $(Preprocessors)
CFLAGS   :=  -g $(Preprocessors)
ASFLAGS  := 
AS       := /usr/bin/as


##
## User defined environment variables
##
CodeLiteDir:=/usr/share/codelite
OLISolution:=/home/sam1203/projects/OLIPlatform/src
Objects0=$(IntermediateDirectory)/src_ChannelFactory.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TLChannel.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TlInStorage.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TLOutStorage.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(ObjectSuffix) $(IntermediateDirectory)/src_TransportLayer.cpp$(ObjectSuffix) 



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
	@echo rebuilt > "/home/sam1203/projects/OLIPlatform/OLIPlatform/.build-debug/TransportLayerLib"

MakeIntermediateDirs:
	@test -d ./Debug || $(MakeDirCommand) ./Debug


./Debug:
	@test -d ./Debug || $(MakeDirCommand) ./Debug

PreBuild:


##
## Objects
##
$(IntermediateDirectory)/src_ChannelFactory.cpp$(ObjectSuffix): src/ChannelFactory.cpp $(IntermediateDirectory)/src_ChannelFactory.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/ChannelFactory.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_ChannelFactory.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_ChannelFactory.cpp$(DependSuffix): src/ChannelFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_ChannelFactory.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_ChannelFactory.cpp$(DependSuffix) -MM "src/ChannelFactory.cpp"

$(IntermediateDirectory)/src_ChannelFactory.cpp$(PreprocessSuffix): src/ChannelFactory.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_ChannelFactory.cpp$(PreprocessSuffix) "src/ChannelFactory.cpp"

$(IntermediateDirectory)/src_TLChannel.cpp$(ObjectSuffix): src/TLChannel.cpp $(IntermediateDirectory)/src_TLChannel.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/TLChannel.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TLChannel.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TLChannel.cpp$(DependSuffix): src/TLChannel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TLChannel.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TLChannel.cpp$(DependSuffix) -MM "src/TLChannel.cpp"

$(IntermediateDirectory)/src_TLChannel.cpp$(PreprocessSuffix): src/TLChannel.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TLChannel.cpp$(PreprocessSuffix) "src/TLChannel.cpp"

$(IntermediateDirectory)/src_TlInStorage.cpp$(ObjectSuffix): src/TlInStorage.cpp $(IntermediateDirectory)/src_TlInStorage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/TlInStorage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TlInStorage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TlInStorage.cpp$(DependSuffix): src/TlInStorage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TlInStorage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TlInStorage.cpp$(DependSuffix) -MM "src/TlInStorage.cpp"

$(IntermediateDirectory)/src_TlInStorage.cpp$(PreprocessSuffix): src/TlInStorage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TlInStorage.cpp$(PreprocessSuffix) "src/TlInStorage.cpp"

$(IntermediateDirectory)/src_TLOutStorage.cpp$(ObjectSuffix): src/TLOutStorage.cpp $(IntermediateDirectory)/src_TLOutStorage.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/TLOutStorage.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TLOutStorage.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TLOutStorage.cpp$(DependSuffix): src/TLOutStorage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TLOutStorage.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TLOutStorage.cpp$(DependSuffix) -MM "src/TLOutStorage.cpp"

$(IntermediateDirectory)/src_TLOutStorage.cpp$(PreprocessSuffix): src/TLOutStorage.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TLOutStorage.cpp$(PreprocessSuffix) "src/TLOutStorage.cpp"

$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(ObjectSuffix): src/TLXMLConfigParser.cpp $(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/TLXMLConfigParser.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(DependSuffix): src/TLXMLConfigParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(DependSuffix) -MM "src/TLXMLConfigParser.cpp"

$(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(PreprocessSuffix): src/TLXMLConfigParser.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TLXMLConfigParser.cpp$(PreprocessSuffix) "src/TLXMLConfigParser.cpp"

$(IntermediateDirectory)/src_TransportLayer.cpp$(ObjectSuffix): src/TransportLayer.cpp $(IntermediateDirectory)/src_TransportLayer.cpp$(DependSuffix)
	$(CXX) $(IncludePCH) $(SourceSwitch) "/home/sam1203/projects/OLIPlatform/src/TransportLayerLib/src/TransportLayer.cpp" $(CXXFLAGS) $(ObjectSwitch)$(IntermediateDirectory)/src_TransportLayer.cpp$(ObjectSuffix) $(IncludePath)
$(IntermediateDirectory)/src_TransportLayer.cpp$(DependSuffix): src/TransportLayer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) -MG -MP -MT$(IntermediateDirectory)/src_TransportLayer.cpp$(ObjectSuffix) -MF$(IntermediateDirectory)/src_TransportLayer.cpp$(DependSuffix) -MM "src/TransportLayer.cpp"

$(IntermediateDirectory)/src_TransportLayer.cpp$(PreprocessSuffix): src/TransportLayer.cpp
	@$(CXX) $(CXXFLAGS) $(IncludePCH) $(IncludePath) $(PreprocessOnlySwitch) $(OutputSwitch) $(IntermediateDirectory)/src_TransportLayer.cpp$(PreprocessSuffix) "src/TransportLayer.cpp"


-include $(IntermediateDirectory)/*$(DependSuffix)
##
## Clean
##
clean:
	$(RM) -r ./Debug/


