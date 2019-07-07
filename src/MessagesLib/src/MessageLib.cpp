#include "OLI_MsgDefines.h"
#include "src/protobuf/oli_messageslib.pb.h"

#include <google/protobuf/stubs/common.h>

using namespace oli::msg;


void MessageLib::init()
{
    GOOGLE_PROTOBUF_VERIFY_VERSION;
}

void MessageLib::deinit()
{
    google::protobuf::ShutdownProtobufLibrary();
}

ApplicationMsg::ApplicationMsg():
    type_(invalid_messageType), buffer_(nullptr), bufferLen_(0)
{}
/*ApplicationMsg::ApplicationMsg(int type, const std::string &body):
    type_(type), body_(body)
{}*/

ApplicationMsg::ApplicationMsg(int type, const char *buffer, size_t bufferLen):
    type_(type), buffer_(buffer), bufferLen_(bufferLen)
{

}

ApplicationMsg::ApplicationMsg(const ApplicationMsg &msg)
{
    type_ = msg.type_;
    buffer_ = msg.buffer_;
    bufferLen_ = msg.bufferLen_;
    //body_ = msg.body_;
}

ApplicationMsg::~ApplicationMsg(){}
