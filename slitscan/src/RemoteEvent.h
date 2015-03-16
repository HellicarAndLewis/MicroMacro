//
//  RemoteEvent.h
//  slitscan
//
//  Created by Chris Mullany on 16/03/2015.
//
//
#pragma once
#include "ofMain.h"


class RemoteEvent : public ofEventArgs {
    
public:
    enum Type {SWITCH_MODE};
    Type type;
    string groupIdsString;
    
    RemoteEvent(Type type, string groupIdsString) {
        this->type = type;
        this->groupIdsString = groupIdsString;
    }
    
    
    // for app wide events, use the static events property below e.g.
    //
    //      ofAddListener(RemoteEvent::events, this, &Class::onRemoteEvent);
    //
    // otherwise, create a new event and args scoped to your class
    
    
    static ofEvent <RemoteEvent> events;
};