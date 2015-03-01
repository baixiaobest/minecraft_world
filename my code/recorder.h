//
//  recorder.h
//  assignment_2_baixiao
//
//  Created by Baixiao Huang on 2/28/15.
//  Copyright (c) 2015 Baixiao Huang. All rights reserved.
//

#ifndef assignment_2_baixiao_recorder_h
#define assignment_2_baixiao_recorder_h

#include <vector>
#include <string>
#include <stdio.h>
#include <sstream>
#include "Utilities.h"

class Recorder{
private:
    std::vector<float> m_time;
    std::vector<point2> m_camera;
    std::vector<point3> m_translation;
    std::vector<point3> m_rotation;
    std::vector<std::string> m_instruction;
    
    int m_pointer;
    int m_size;
public:
    Recorder(){m_pointer = 0; m_size = 0;}
    bool loadFile(std::string);
    bool saveFile(std::string);
    
    //logger
    void logCam(float time, point2);
    void logTranslation(float time, point3);
    void logRotation(float time, point3);
    void logInstruction(float time, std::string);
    
    //getter
    float getTime(){return m_time[m_pointer];};
    point2 getCam(){return m_camera[m_pointer];};
    point3 getTranslation(){return m_translation[m_pointer];};
    point3 getRotation(){return m_rotation[m_pointer];};
    std::string getInstruction(){return m_instruction[m_pointer];};
    bool pop();
    
};

inline
bool Recorder::loadFile(std::string fileName)
{
    std::ifstream myFile(fileName);
    std::string line;
    while (std::getline(myFile, line)) {
        std::string data[10];
        for (int i=0; i<10; i++) {
            data[i] = line.substr(0,line.find(","));
            line = line.substr(line.find(",")+1, line.length());
        }
        m_time.push_back(stof(data[0]));
        m_camera.push_back(point2(stof(data[1]), stof(data[2])));
        m_translation.push_back(point3(stof(data[3]), stof(data[4]), stof(data[5])));
        m_rotation.push_back(point3(stof(data[6]), stof(data[7]),stof(data[8])));
        m_instruction.push_back(data[9]);
        
        printf("%f %f %f %f %f %f %f %f %f %s\n",m_time.back(), m_camera.back().x, m_camera.back().y, m_translation.back().x, m_translation.back().y, m_translation.back().z, m_rotation.back().x, m_rotation.back().y, m_rotation.back().z, m_instruction.back().c_str());
        
        m_size++;
    }
    return true;
}

bool Recorder::saveFile(std::string outputFile)
{
    std::ofstream outputStream(outputFile);
    for (int i=0; i<m_size; i++) {
        std::string line_time = std::to_string(m_time[i])+",";
        std::string line_cam = std::to_string(m_camera[i].x)+","+std::to_string(m_camera[i].y);
        std::string line_trans = std::to_string(m_translation[i].x)+","+std::to_string(m_translation[i].y)+","+std::to_string(m_translation[i].z)+",";
        std::string line_rotate = std::to_string(m_rotation[i].x)+","+std::to_string(m_rotation[i].y)+","+std::to_string(m_rotation[i].z)+",";
        std::string line_instruct = m_instruction[i]+"\n";
        std::string outputStr = line_time+line_cam+line_trans+line_rotate+line_instruct;
        outputStream.write(outputStr.c_str(), sizeof(char)*outputStr.length());
    }
    return true;
}

bool Recorder::pop()
{
    if (m_size == 0) {
        return false;
    }else{
        m_pointer++;
        m_size--;
        return true;
    }
}

void Recorder::logCam(float time, point2 cam)
{
    m_time.push_back(time);
    m_camera.push_back(cam);
    m_translation.push_back(point3(0,0,0));
    m_rotation.push_back(point3(0,0,0));
    m_instruction.push_back("0");
    m_size++;
}

void Recorder::logTranslation(float time, point3 trans)
{
    m_time.push_back(time);
    m_translation.push_back(trans);
    m_camera.push_back(point2(0,0));
    m_rotation.push_back(point3(0,0,0));
    m_instruction.push_back("0");
    m_size++;
}

void Recorder::logRotation(float time, point3 rotation)
{
    m_time.push_back(time);
    m_rotation.push_back(rotation);
    m_camera.push_back(point2(0,0));
    m_translation.push_back(point3(0,0,0));
    m_instruction.push_back("0");
    m_size++;
}

void Recorder::logInstruction(float time, std::string instruction)
{
    m_time.push_back(time);
    m_instruction.push_back(instruction);
    m_camera.push_back(point2(0,0));
    m_translation.push_back(point3(0,0,0));
    m_rotation.push_back(point3(0,0,0));
    m_size++;
}

#endif
