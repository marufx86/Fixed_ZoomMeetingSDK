// Copyright : All rights reserved by Zoom Video Communications 2022-

#pragma once

#include "ZoomUESDKDef.h"
#include "meeting_service_components/meeting_chat_interface.h"

BEGIN_ZOOM_SDK_UE_NAMESPACE

// A partial implementation of IChatMsgInfo.
// More info:
// https://marketplacefront.zoom.us/sdk/meeting/windows/class_i_chat_msg_info.html
class ChatMsgInfo : public IChatMsgInfo {
public:
  ChatMsgInfo(wchar_t *content, unsigned int receiver, SDKChatMessageType type)
      : m_receiver_user_id(receiver), m_content(content), 
        m_chat_message_type(type) {}

  // functions to override
  const zchar_t *GetMessageID() { return m_message_id; }
  unsigned int GetSenderUserId() { return m_sender_user_id; }
  const zchar_t *GetSenderDisplayName() { return m_sender_display_name; }
  unsigned int GetReceiverUserId() { return m_receiver_user_id; }
  const zchar_t *GetReceiverDisplayName() { return m_receiver_display_name; }
  const zchar_t *GetContent() { return m_content; }
  time_t GetTimeStamp() { return m_timestamp; }
  bool IsChatToAll() { return m_is_chat_to_all; }
  bool IsChatToAllPanelist() { return m_is_chat_all_panelist; }
  bool IsChatToWaitingroom() { return m_is_chat_to_waiting_room; }
  SDKChatMessageType GetChatMessageType() { return m_chat_message_type; }
  bool IsComment() { return m_is_comment; }
  bool IsThread() { return m_is_thread; }
  IList<IRichTextStyleItem *> *GetTextStyleItemList() { return nullptr; }
  IList<SegmentDetails> *GetSegmentDetails() { return nullptr; }
  const zchar_t *GetThreadID() { return m_thread_id; }

private:
  zchar_t *m_message_id;
  unsigned int m_sender_user_id;
  zchar_t *m_sender_display_name = nullptr;
  unsigned int m_receiver_user_id;
  zchar_t *m_receiver_display_name = nullptr;
  zchar_t *m_content = nullptr;
  time_t m_timestamp;
  bool m_is_chat_to_all = false;
  bool m_is_chat_all_panelist = false;
  bool m_is_chat_to_waiting_room = false;
  SDKChatMessageType m_chat_message_type = SDKChatMessageType_To_None;
  bool m_is_comment = false;
  bool m_is_thread = false;
  zchar_t *m_thread_id = nullptr;
};

/**
 * Wrapper around IMeetingChatController
 * Check
 * https://marketplacefront.zoom.us/sdk/meeting/windows/class_i_meeting_chat_controller.html
 * for interface definition.
 */
class ZOOMMEETINGSDK_API MeetingChatController {
public:
  ~MeetingChatController() = default;
  MeetingChatController(const MeetingChatController &other) = delete;
  MeetingChatController(MeetingChatController &&other) noexcept = delete;
  MeetingChatController &operator=(const MeetingChatController &other) = delete;
  MeetingChatController &
  operator=(MeetingChatController &&other) noexcept = delete;

  // Static function to create an object of IMeetingChatController.
  static MeetingChatController *Create();

  // Implementation of the required methods.
  const ChatStatus *GetChatStatus();
  SDKError SendChatMsgTo(wchar_t *content, unsigned int receiver,
                         SDKChatMessageType type);
  bool IsMeetingChatLegalNoticeAvailable();
  const wchar_t *GetChatLegalNoticesPrompt();
  const wchar_t *GetChatLegalNoticesExplained();
  bool IsShareMeetingChatLegalNoticeAvailable();
  const wchar_t *GetShareMeetingChatStartedLegalNoticeContent();
  const wchar_t *GetShareMeetingChatStoppedLegalNoticeContent();
  bool IsChatMessageCanBeDeleted(const wchar_t *msgID);
  SDKError DeleteChatMessage(const wchar_t *msgID);
  IList<const wchar_t *> *GetAllChatMessageID();
  IChatMsgInfo *GetChatMessageById(const wchar_t *msgID);

private:
  MeetingChatController() = default;

  // Initializes MeetingChatController.
  //
  // Returns true if initialization succeeds.
  bool Init();

  IMeetingChatController *meeting_chat_controller;
};
END_ZOOM_SDK_UE_NAMESPACE
