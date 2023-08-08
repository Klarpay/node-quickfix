/*
 * FixMessageUtil.h
 *
 *  Created on: Sep 8, 2014
 *      Author: kdeol
 */

#ifndef FIXMESSAGEUTIL_H_
#define FIXMESSAGEUTIL_H_

#include <v8.h>
#include <node.h>
#include <nan.h>

#include "FixEvent.h"
#include "quickfix/SessionID.h"
#include "quickfix/Message.h"

using namespace node;

class FixMessageUtil {
public:
	FixMessageUtil();
	virtual ~FixMessageUtil();

	static void addFixHeader(FIX::Message* message, Local<v8::Object> msg) {
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();

    Local<v8::String> key = Nan::New<v8::String>("header").ToLocalChecked();
		Local<v8::Object> header = Local<v8::Object>::Cast(msg->Get(context, key).ToLocalChecked());
		FIX::Header &msgHeader = message->getHeader();
		Local<v8::Array> headerTags = header->GetPropertyNames(context).ToLocalChecked();

		for(int i=0; i < (int)headerTags->Length(); i++) {
			String::Utf8Value value(v8::Isolate::GetCurrent(), header->Get(context, headerTags->Get(context, i).ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());
			msgHeader.setField(
					headerTags->Get(context, i).ToLocalChecked()->Int32Value(context).ToChecked(),
					std::string(*value)
			);
		}
	}

	static void addFixTags(FIX::FieldMap* map, Local<v8::Object> msg) {
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		Local<v8::String> tagsKey = Nan::New<v8::String>("tags").ToLocalChecked();

		if(msg->Has(context, tagsKey).ToChecked()) {

			Local<v8::Object> tags = Local<v8::Object>::Cast(msg->Get(context, tagsKey).ToLocalChecked());

			Local<v8::Array> msgTags = tags->GetPropertyNames(context).ToLocalChecked();

			for(int i=0; i < (int)msgTags->Length(); i++) {
				String::Utf8Value value(v8::Isolate::GetCurrent(), tags->Get(context, msgTags->Get(context, i).ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());


				map->setField(
						msgTags->Get(context, i).ToLocalChecked()->Int32Value(context).ToChecked(),
						std::string(*value)
				);
			}

		}

	}

	static void addFixGroups(FIX::FieldMap* map, Local<v8::Object> msg) {
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		Local<v8::String> groupKey = Nan::New<v8::String>("groups").ToLocalChecked();

		// TODO: add type checking and dev-helpful error throwing


		if(msg->Has(context, groupKey).ToChecked()) {


			Local<v8::Array> groups = Local<v8::Array>::Cast(msg->Get(context, groupKey).ToLocalChecked());

			for(int i = 0; i < (int) groups->Length(); i++) {


				Local<v8::Object> groupObj = groups->Get(context, i).ToLocalChecked()->ToObject(context).ToLocalChecked();

				Local<v8::String> delimKey = Nan::New<v8::String>("delim").ToLocalChecked();
				Local<v8::String> indexKey = Nan::New<v8::String>("index").ToLocalChecked();

				if( ! groupObj->Has(context, indexKey).ToChecked()) {
						Nan::ThrowError("no index property found on object");
				}

				if( ! groupObj->Has(context, delimKey).ToChecked()) {
						Nan::ThrowError("no delim property found on object");
				}


				Local<v8::String> entriesKey = Nan::New<v8::String>("entries").ToLocalChecked();

				if( ! groupObj->Has(context, entriesKey).ToChecked()) {
						Nan::ThrowError("no entries property found on object");
				}

				Local<v8::Array> groupEntries = Local<v8::Array>::Cast(groupObj->Get(context, entriesKey).ToLocalChecked());

				for (int j = 0; j < (int) groupEntries->Length(); j++) {

					Local<v8::Object> entry = groupEntries->Get(context, j).ToLocalChecked()->ToObject(context).ToLocalChecked();

					Local<v8::String> tagKey = Nan::New<v8::String>("tags").ToLocalChecked();

					if(entry->Has(context, groupKey).ToChecked() || entry->Has(context, tagKey).ToChecked()) {

						FIX::Group* group = new FIX::Group(
							groupObj->Get(context, indexKey).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value(),
							groupObj->Get(context, delimKey).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value());

						addFixTags(group, entry);
						addFixGroups(group, entry);

						map->addGroup(group->field(), *group);

						delete group;

					} else {

						FIX::Group* group = new FIX::Group(
							groupObj->Get(context, indexKey).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value(),
							groupObj->Get(context, delimKey).ToLocalChecked()->ToInteger(context).ToLocalChecked()->Value());

						// compat for old, non-nested format

						Local<v8::Array> entryTags = entry->GetPropertyNames(context).ToLocalChecked();

						for(int k=0; k < (int) entryTags->Length(); k++) {

							Local<v8::String> prop = entryTags->Get(context, k).ToLocalChecked()->ToString(context).ToLocalChecked();
							String::Utf8Value keyStr(v8::Isolate::GetCurrent(), prop->ToString(context).ToLocalChecked());
							String::Utf8Value valueStr(v8::Isolate::GetCurrent(), entry->Get(context, prop).ToLocalChecked()->ToString(context).ToLocalChecked());

							group->setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));

						}

						map->addGroup(group->field(), *group);

						delete group;

					}
				}
			}
		}
	}

	static void addFixTrailer(FIX::Message* message, Local<v8::Object> msg) {
    auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		FIX::Trailer &msgTrailer = message->getTrailer();

		Local<v8::String> trailerKey = Nan::New<v8::String>("trailer").ToLocalChecked();

		if(msg->Has(context, trailerKey).ToChecked()) {

			Local<v8::Object> trailer = Local<v8::Object>::Cast(msg->Get(context, trailerKey).ToLocalChecked());
			Local<v8::Array> trailerTags = trailer->GetPropertyNames(context).ToLocalChecked();

			for(int i=0; i<(int)trailerTags->Length(); i++) {

				Local<v8::String> prop = trailerTags->Get(context, i).ToLocalChecked()->ToString(context).ToLocalChecked();
				String::Utf8Value keyStr(v8::Isolate::GetCurrent(), prop->ToString(context).ToLocalChecked());

				String::Utf8Value valueStr(v8::Isolate::GetCurrent(), trailer->Get(context, prop).ToLocalChecked()->ToString(context).ToLocalChecked());

				msgTrailer.setField(atoi(std::string(*keyStr).c_str()), std::string(*valueStr));

			}

		}

	}

	static void js2Fix(FIX::Message* message, Local<v8::Object> msg) {

		addFixHeader(message, msg);

		addFixTags(message, msg);

		addFixGroups(message, msg);

		addFixTrailer(message, msg);

	}

	static void addJsHeader(Local<v8::Object> msg, const FIX::Message* message) {
		Local<v8::Object> header = Nan::New<v8::Object>();
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		FIX::Header messageHeader = message->getHeader();

		for(FIX::FieldMap::iterator it = messageHeader.begin(); it != messageHeader.end(); ++it)
		{
			header->Set(context, Nan::New<Integer>(it->getTag()), Nan::New<v8::String>(it->getString().c_str()).ToLocalChecked());
		}

		msg->Set(context, Nan::New<v8::String>("header").ToLocalChecked(), header);
	}

	static void addJsTags(Local<v8::Object> msg, const FIX::FieldMap* map) {
		Local<v8::Object> tags = Nan::New<v8::Object>();
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		int noTags = 0;

		for(auto it = map->begin(); it != map->end(); ++it)
		{
			tags->Set(context, Nan::New<Integer>(it->getTag()), Nan::New<v8::String>(it->getString().c_str()).ToLocalChecked());
			noTags++;
		}

		if (noTags > 0) {
			msg->Set(context, Nan::New<v8::String>("tags").ToLocalChecked(), tags);
		}
	}

	static void addJsTrailer(Local<v8::Object> msg, const FIX::Message* message) {
		Local<v8::Object> trailer = Nan::New<v8::Object>();
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		FIX::Trailer messageTrailer = message->getTrailer();

		for(FIX::FieldMap::iterator it = messageTrailer.begin(); it != messageTrailer.end(); ++it)
		{
			trailer->Set(context, Nan::New<Integer>(it->getTag()), Nan::New<v8::String>(it->getString().c_str()).ToLocalChecked());
		}

		msg->Set(context, Nan::New<v8::String>("trailer").ToLocalChecked(), trailer);
	}

	static void addJsGroups(Local<v8::Object> msg, const FIX::FieldMap* map) {
		Local<v8::Object> groups = Nan::New<v8::Object>();
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		int noGroups = 0;

		for(auto it = map->g_begin(); it != map->g_end(); ++it) {
			std::vector< FIX::FieldMap* > groupVector = it->second;
			auto groupList = Nan::New<v8::Array>(groupVector.size());
			int i = 0;

			for(std::vector< FIX::FieldMap* >::iterator v_it = groupVector.begin(); v_it != groupVector.end(); ++v_it) {
				Local<v8::Object> groupEntry = Nan::New<v8::Object>();

				FIX::FieldMap* fields = *v_it;

				addJsTags(groupEntry, fields);

				addJsGroups(groupEntry, fields);

				groupList->Set(context, i, groupEntry);

				i++;
			}

			groups->Set(context, Nan::New<Integer>(it->first), groupList);

			noGroups++;
		}

		if (noGroups > 0) {
			msg->Set(context, Nan::New<v8::String>("groups").ToLocalChecked(), groups);
		}
	}

	static void fix2Js(Local<v8::Object> msg, const FIX::Message* message) {

		addJsHeader(msg, message);

		addJsTags(msg, message);

		addJsTrailer(msg, message);

		addJsGroups(msg, message);

	}

	static Local<Value> sessionIdToJs(const FIX::SessionID* sessionId) {
		Local<v8::Object> session = Nan::New<v8::Object>();
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();

		session->Set(context, Nan::New<v8::String>("beginString").ToLocalChecked(), Nan::New<v8::String>(sessionId->getBeginString().getString().c_str()).ToLocalChecked());
		session->Set(context, Nan::New<v8::String>("senderCompID").ToLocalChecked(), Nan::New<v8::String>(sessionId->getSenderCompID().getString().c_str()).ToLocalChecked());
		session->Set(context, Nan::New<v8::String>("targetCompID").ToLocalChecked(), Nan::New<v8::String>(sessionId->getTargetCompID().getString().c_str()).ToLocalChecked());
		session->Set(context, Nan::New<v8::String>("sessionQualifier").ToLocalChecked(), Nan::New<v8::String>(sessionId->getSessionQualifier().c_str()).ToLocalChecked());

		return session;
	}

	static FIX::SessionID jsToSessionId(Local<v8::Object> sessionId) {
		auto context = v8::Isolate::GetCurrent()->GetCurrentContext();
		String::Utf8Value beginString(v8::Isolate::GetCurrent(), sessionId->Get(context, Nan::New<v8::String>("beginString").ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());
		String::Utf8Value senderCompId(v8::Isolate::GetCurrent(), sessionId->Get(context, Nan::New<v8::String>("senderCompID").ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());
		String::Utf8Value targetCompId(v8::Isolate::GetCurrent(), sessionId->Get(context, Nan::New<v8::String>("targetCompID").ToLocalChecked()).ToLocalChecked()->ToString(context).ToLocalChecked());
		return FIX::SessionID(std::string(*beginString),
				std::string(*senderCompId),
				std::string(*targetCompId));
	}
};

#endif /* FIXMESSAGEUTIL_H_ */
