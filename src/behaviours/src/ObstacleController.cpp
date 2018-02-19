#include "ObstacleController.h"
#include <cmath>
#include <iostream>

ObstacleController::ObstacleController()
{
  obstacleAvoided = true;
  obstacleDetected = false;
  obstacleInterrupt = false;
  result.PIDMode = CONST_PID; //use the const PID to turn at a constant speed
}

//note, not a full reset as this could cause a bad state
//resets the interupt and knowledge of an obstacle or obstacle avoidance only.
void ObstacleController::Reset() {
  obstacleAvoided = true;
  obstacleDetected = false;
  obstacleInterrupt = false;
  delay = current_time;
}

bool ObstacleController::getObstacleInfo()
{
  return obstacleDetected;
}

//Return true if the rover can move to desired location
bool ObstacleController::rfree() {
  if (right > 0.8){
    return true;
  }
  return false;
}

bool ObstacleController::lfree() {
  if (left > 0.8){
    return true;
  }
  return false;
}
// Avoid crashing into objects detected by the ultraound
// void ObstacleController::avoidObstacle() {

//     //always turn right to avoid obstacles
//     if (right < 0.8 || center < 0.8 || left < 0.8) {
//       cout << "Pared encontrada en (" << currentLocation.x << "," << currentLocation.y << ")";
//       obstacleDetected = true;
//       result.type = precisionDriving;

//       result.pd.cmdAngular = -K_angular;

//       result.pd.setPointVel = 0.0;
//       result.pd.cmdVel = 0.0;
//       result.pd.setPointYaw = 0;
//     }
// }

void ObstacleController::follow_Wall() {
  

    //always turn right to avoid obstacles
    //if (right < 0.8 || center < 0.8 || left < 0.8) {
      obstacleDetected = true;
      // result.type = precisionDriving;
      //result.pd.cmdAngular = -K_angular;

      // result.pd.setPointVel = 0.0;
      // result.pd.cmdVel = 0.0;
      // result.pd.setPointYaw = 0;

      if (right < 0.8 && center < 0.8){
        caseVar = 1;
        cout <<  "Im in Case 1!" << endl;
      }
       else if (left < 0.8 && center < 0.8){
        caseVar = 2;
        cout <<  "Im in Case 2!" << endl;
       }
       else if (right < 0.8){
        caseVar = 3;
        cout <<  "Im in Case 3!" << endl;
       }
       else if (left < 0.8){
        caseVar = 4;
        cout <<  "Im in Case 4!" << endl;
       }

    //}
}

// A collection zone was seen in front of the rover and we are not carrying a target
// so avoid running over the collection zone and possibly pushing cubes out.
void ObstacleController::avoidCollectionZone() {

    cout << "Base detected" << endl;
    result.type = precisionDriving;

    result.pd.cmdVel = 0.0;

    // Decide which side of the rover sees the most april tags and turn away
    // from that side
    if(count_left_collection_zone_tags < count_right_collection_zone_tags) {
      result.pd.cmdAngular = K_angular;
    } else {
      result.pd.cmdAngular = -K_angular;
    }

    result.pd.setPointVel = 0.0;
    result.pd.cmdVel = 0.0;
    result.pd.setPointYaw = 0;
}


Result ObstacleController::DoWork() {
  double rightAngle = ((int(currentLocation.theta) - 30) + 180) % 360;
  double leftAngle = ((int(currentLocation.theta) + 30) + 180) % 360;
  clearWaypoints = true;
  set_waypoint = true;
  result.PIDMode = CONST_PID;

  // The obstacle is an april tag marking the collection zone
  if(collection_zone_seen){
    avoidCollectionZone();
  }
  else {
    //avoidObstacle();
    follow_Wall();
  }

  //if an obstacle has been avoided
  if (can_set_waypoint) {

    can_set_waypoint = false; //only one waypoint is set
    set_waypoint = false;
    clearWaypoints = false;
    obstacleDetected = false;
    result.type = waypoint;
    result.PIDMode = FAST_PID; //use fast pid for waypoints
    Point V1, V2, V3;
    
    switch (caseVar) {
      case 1:
      //while (right < 0.8 && center < 0.8){
        V1.x = right * cos(rightAngle);
        V1.y = right * sin(rightAngle);
        V2.x = center * cos(rightAngle);
        V2.y = center * sin(rightAngle);

        V3.x = currentLocation.x + V1.x + V2.x;
        V3.y = currentLocation.y + V1.y + V2.y;

        cout << "X value for new vector = " << V3.x << endl;
        cout << "Y value for new vector = " << V3.y << endl;

        result.wpts.waypoints.clear();
        result.wpts.waypoints.push_back(V3);
      //}
      break;
    
      case 2:
      V1.x = left * cos(leftAngle);
      V1.y = left * sin(leftAngle);
      V2.x = center * cos(leftAngle);
      V2.y = center * sin(leftAngle);

      V3.x = V1.x + V2.x + currentLocation.x;
      V3.y = V1.y + V2.y + currentLocation.y;
      result.wpts.waypoints.clear();
      result.wpts.waypoints.push_back(V3);
      break;

    // else if (right < 0.8){
    //   V1.x = right * cos(rightAngle);
    //   V1.y = right * sin(rightAngle);

    //   V2.x = V1.x + currentLocation.x;
    //   V2.y = V1.y + currentLocation.y;
    //   result.wpts.waypoints.clear();
    //   result.wpts.waypoints.push_back(V2);
    // }
    // else if (left < 0.8){
    //   V1.x = left * cos(leftAngle);
    //   V1.y = left * sin(leftAngle);

    //   V2.x = V1.x + currentLocation.x;
    //   V2.y = V1.y + currentLocation.y;
    //   result.wpts.waypoints.clear();
    //   result.wpts.waypoints.push_back(V2);
    // }
    // Point forward; 
    // //waypoint is directly ahead of current heading
    // forward.x = currentLocation.x + (0.5 * cos(currentLocation.theta));
    // forward.y = currentLocation.y + (0.5 * sin(currentLocation.theta));  
    // result.wpts.waypoints.clear();
    // result.wpts.waypoints.push_back(forward);

    // //set heading to previous state
    // result.type = precisionDriving;
    // result.pd.cmdAngular = K_angular;
    // result.pd.setPointVel = 0.0;
    // result.pd.cmdVel = 0.0;
    // result.pd.setPointYaw = 0;
    

    // result.type = waypoint;
    // result.PIDMode = FAST_PID;
    // forward.x = currentLocation.x + (2 * cos(currentLocation.theta));
    // forward.y = currentLocation.y + (2 * sin(currentLocation.theta));  
    // result.wpts.waypoints.clear();
    // result.wpts.waypoints.push_back(forward);
    }
  }


  return result;
}


void ObstacleController::setSonarData(float sonarleft, float sonarcenter, float sonarright) {
  left = sonarleft;
  right = sonarright;
  center = sonarcenter;

  ProcessData();
}

void ObstacleController::setCurrentLocation(Point currentLocation) {
  this->currentLocation = currentLocation;
}

void ObstacleController::ProcessData() {

  //timeout timer for no tag messages
  //this is used to set collection zone seen to false beacuse
  //there is no report of 0 tags seen
  long int Tdifference = current_time - timeSinceTags;
  float Td = Tdifference/1e3;
  if (Td >= 0.5) {
    collection_zone_seen = false;
    phys= false;
    if (!obstacleAvoided)
    {
      can_set_waypoint = true;
    }
  }

  //If we are ignoring the center sonar
  if(ignore_center_sonar){
    //If the center distance is longer than the reactivation threshold
    if(center > reactivate_center_sonar_threshold){
      //currently do not re-enable the center sonar instead ignore it till the block is dropped off
      //ignore_center_sonar = false; //look at sonar again beacuse center ultrasound has gone long
    }
    else{
      //set the center distance to "max" to simulated no obstacle
      center = 3;
    }
  }
  else {
    //this code is to protect against a held block causing a false short distance
    //currently pointless due to above code
    if (center < 3.0) {
      result.wristAngle = 0.7;
    }
    else {
      result.wristAngle = -1;
    }
  }

  //if any sonar is below the trigger distance set physical obstacle true
  if (left < triggerDistance || right < triggerDistance || center < triggerDistance)
  {
    phys = true;
    timeSinceTags = current_time;
  }

  //if physical obstacle or collection zone visible
  if (collection_zone_seen || phys)
  {
    obstacleDetected = true;
    obstacleAvoided = false;
    can_set_waypoint = false;
  }
  else
  {
    obstacleAvoided = true;
  }
}

// Report April tags seen by the rovers camera so it can avoid
// the collection zone
// Added relative pose information so we know whether the
// top of the AprilTag is pointing towards the rover or away.
// If the top of the tags are away from the rover then treat them as obstacles.
void ObstacleController::setTagData(vector<Tag> tags){
  collection_zone_seen = false;
  count_left_collection_zone_tags = 0;
  count_right_collection_zone_tags = 0;

  // this loop is to get the number of center tags
  if (!targetHeld) {
    for (int i = 0; i < tags.size(); i++) { //redundant for loop
      if (tags[i].getID() == 256) {

	collection_zone_seen = checkForCollectionZoneTags( tags );
        timeSinceTags = current_time;
      }
    }
  }
}

bool ObstacleController::checkForCollectionZoneTags( vector<Tag> tags ) {

  for ( auto & tag : tags ) {

    // Check the orientation of the tag. If we are outside the collection zone the yaw will be positive so treat the collection zone as an obstacle.
    //If the yaw is negative the robot is inside the collection zone and the boundary should not be treated as an obstacle.
    //This allows the robot to leave the collection zone after dropping off a target.
    if ( tag.calcYaw() > 0 )
      {
	// checks if tag is on the right or left side of the image
	if (tag.getPositionX() + camera_offset_correction > 0) {
	  count_right_collection_zone_tags++;

	} else {
	  count_left_collection_zone_tags++;
	}
      }

  }


  // Did any tags indicate that the robot is inside the collection zone?
  return count_left_collection_zone_tags + count_right_collection_zone_tags > 0;

}

//obstacle controller should inrerupt is based upon the transition from not seeing and obstacle to seeing an obstacle
bool ObstacleController::ShouldInterrupt() {

  //if we see and obstacle and havent thrown an interrupt yet
  if(obstacleDetected && !obstacleInterrupt)
  {
    obstacleInterrupt = true;
    return true;
  }
  else
  {
    //if the obstacle has been avoided and we had previously detected one interrupt to change to waypoints
    if(obstacleAvoided && obstacleDetected)
    {
      Reset();
      return true;
    } else {
      return false;
    }
  }
}

bool ObstacleController::HasWork() {
  //there is work if a waypoint needs to be set or the obstacle hasnt been avoided
  if (can_set_waypoint && set_waypoint)
  {
    return true;
  }

  return !obstacleAvoided;
}

//ignore center ultrasound
void ObstacleController::setIgnoreCenterSonar(){
  ignore_center_sonar = true;
}

void ObstacleController::setCurrentTimeInMilliSecs( long int time )
{
  current_time = time;
}

void ObstacleController::setTargetHeld() {
  targetHeld = true;

  //adjust current state on transition from no cube held to cube held
  if (previousTargetState == false) {
    obstacleAvoided = true;
    obstacleInterrupt = false;
    obstacleDetected = false;
    previousTargetState = true;
  }
}

void ObstacleController::setTargetHeldClear()
{
  //adjust current state on transition from cube held to cube not held
  if (targetHeld)
  {
    Reset();
    targetHeld = false;
    previousTargetState = false;
    ignore_center_sonar = false;
  }
}
