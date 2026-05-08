#pragma once

#include "figure.hxx"
#include "sequence.hxx"
#include <fstream>
#include <raylib.h>
#include <sstream>
#include <string>

bool writeSequenceToFile(Sequence& sequence, std::string filename) {
    std::stringstream stream{};

    for (int i = 0; i < sequence.size(); i++) {
        Pose& pose = sequence[i];
        stream << "POSE\n" << i << "\n";
        for (const auto& pair : pose.positionMap) {
            stream << "POSITION\n" << pair.first << "\n";
            stream << pair.second.x << "\n" << pair.second.y << "\n";
        }
        for (const auto& pair : pose.rotationMap) {
            stream << pair.first << "\n" << pair.second << "\n";
        }
        stream << "!\n";
    }

    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to stream `" << filename << "` to save sequence!" << std::endl;
        return false;
    }
    file << stream.rdbuf();

    std::cout << "Success: Saved sequence to file `" << filename << "`" << std::endl;
    return true;
}

bool readSequenceFromFile(Sequence& sequence, std::string filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << filename << "` to read sequence!" << std::endl;
        return false;
    }

    sequence.clear();

    Pose newPose{};
    std::string name{};

    std::string s{};
    while (getline(file, s)) {
        if (s == "POSE") {
            getline(file, s);
            newPose = {};
        }
        else if (s == "!") {
            sequence.addAt(newPose);
        }
        else if (s == "POSITION") {
            try {
                getline(file, s); name = s;
                getline(file, s); newPose.positionMap[name].x = std::stof(s);
                getline(file, s); newPose.positionMap[name].y = std::stof(s);
            }
            catch (...) { return false; }
        }
        else {
            name = s;
            try {
                getline(file, s); newPose.rotationMap[name] = std::stof(s);
            }
            catch (...) { return false; }
        }
    }
    std::cout << "Success: Loaded in sequence from file `" << filename << "`" << std::endl;
    return true;
}

bool readFigureFromFile(Figure& figure, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << filename << "`!" << std::endl;
        std::exit(1);
    }

    figure.clear();

    std::vector<std::string> parents{};
    std::string s{};
    while (getline(file, s)) {
        if (s == "+") {
            Part newPart{};

            try {
                getline(file, s); newPart.name = s;
                getline(file, s); newPart.bounds.x = std::stoi(s);
                getline(file, s); newPart.bounds.y = std::stoi(s);
                getline(file, s); newPart.bounds.width = std::stoi(s);
                getline(file, s); newPart.bounds.height = std::stoi(s);
                getline(file, s); newPart.pivot.x = std::stoi(s);
                getline(file, s); newPart.pivot.y = std::stoi(s);
                getline(file, s); parents.push_back(s);
                getline(file, s); newPart.connectedNotch = std::stoi(s);
            
                getline(file, s);
                while (s == ".") {
                    getline(file, s); int x = std::stoi(s);
                    getline(file, s); int y = std::stoi(s);
                    newPart.localNotches.push_back((Vector2){(float)x, (float)y});
                    newPart.worldNotches.push_back((Vector2){(float)x, (float)y});
                    getline(file, s);
                }
            }
            catch (...) {
                return false;
            }
            figure.push_back(newPart);
        }
    }
    for (int i = 0; i < figure.size(); i++) {
        figure[i].parent = figure.getPartByName(parents[i]);
        // We set the parent pointers after the list is complete because setting
        // them while it is being built and resized seems to mess up everything.
        if (figure[i].parent == nullptr) {
            figure[i].position.x = windowWidth / 2;
            figure[i].position.y = windowHeight / 2;
        }
    }
    figure.root = figure.getPartByName(figure.getPose().positionMap.begin()->first);

    file.close();
    return true;
}

bool exportAsSpritesheet(Texture texture, Sequence sequence) {
    RenderTexture target = LoadRenderTexture(windowWidth*sequence.size(), windowHeight);
    Image output{};
    Figure figure{};
    Camera2D camera{};

    readFigureFromFile(figure, partsFilename);

    camera.offset = (Vector2){windowWidth/2, windowHeight/2};
    camera.target = (Vector2){windowWidth/2, windowHeight/2};
    camera.zoom = cameraZoomLevel;

    if (MakeDirectory(outputDirectory) != 0) {
        std::cerr << "Error: Failed to create output directory, `" << outputDirectory << "`" << std::endl;
        UnloadTexture(texture);
        UnloadImage(output);
        return false;
    }

    for (int i = 0; i < sequence.size(); i++) {
        figure.setPose(sequence[i]);
        figure.update();
        BeginTextureMode(target); {
            BeginMode2D(camera); {
                figure.draw(texture, debugColors[i]);
            }
            EndMode2D();
        }
        EndTextureMode();
        camera.offset.x += windowWidth;
    }

    output = LoadImageFromTexture(target.texture);
    ImageFlipVertical(&output); // For some reason, the image loads in flipped

    ExportImage(output, TextFormat("%s/%s-%s.png", outputDirectory, textureFilename, sequenceFilename));

    UnloadRenderTexture(target);
    UnloadImage(output);
    return true;
}
