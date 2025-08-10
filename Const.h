#ifndef CONST_H
#define CONST_H

const unsigned cmNothing = 111;
const unsigned cmGenerateScript = 200;
const unsigned cmDefaultProperties = 201;
const unsigned cmDefaultPayload = 202;
const unsigned cmUpdateTags = 203;
const unsigned cmClearParams = 204;
const unsigned cmOpenRunWindow = 205;
const unsigned cmRunScriptNow  = 206;
const unsigned cmUpdateProgress  = 207;
const unsigned cmAppendLog  = 208;
const unsigned cmScriptFinished  = 209;
const unsigned cmAbort  = 210;
const unsigned cmOpenProject  = 211;
const unsigned cmSaveProject  = 212;


const std::string defaultProperties = R"({
"message_id": "{{RANDOM_UUID}}",
"delivery_mode": 2,
"content_type": "application/json",
"headers": {
    "x-event-class-name": "",
    "x-event-type-key": ""
  }
})";

const std::string defaultPayload = R"({
  "dataUtworzeniaEventu": "{{NOW}}"
})";

#endif //CONST_H
