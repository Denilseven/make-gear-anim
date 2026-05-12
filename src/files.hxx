#pragma once

#include "figure.hxx"
#include "sequence.hxx"
#include <fstream>
#include <raylib.h>
#include <sstream>
#include <string>

inline std::string wordAt(std::string& text, int index = 0) {
    std::string result{};
    std::istringstream istream{text};
    for (int i = -1; i < index; i++)
        std::getline(istream, result, ' ');
    return result;
}

inline bool writeSequenceToFile(Sequence& sequence, std::string filename) {
    std::stringstream stream{};

    for (int i = 0; i < sequence.size(); i++) {
        Pose& pose = sequence[i];
        stream << "POSE " << i << "\n";
        for (const auto& pair : pose.mirrorMap)
            stream << "(mirror) " << pair.first << "\n";
        for (const auto& pair : pose.positionMap)
            stream << "(position) " << pair.first << " " << pair.second.x << " " << pair.second.y << "\n";
        for (const auto& pair : pose.rotationMap)
            stream << "(rotate) " << pair.first << " " << pair.second << "\n";
        stream << "END\n\n";
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

// WARNING: When it fails to read, it simply crashes the program!
// TODO: Add error checking back in!
inline bool readSequenceFromFile(Sequence& sequence, std::string filename) {
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
        if (wordAt(s) == "POSE") {
            newPose = {};
        }
        else if (wordAt(s) == "END") {
            sequence.addAt(newPose);
        }
        else if (wordAt(s) == "(mirror)") {
            name = wordAt(s, 1);
            newPose.mirrorMap[name] = true;
        }
        else if (wordAt(s) == "(position)") {
            name = wordAt(s, 1);
            newPose.positionMap[name].x = std::stof(wordAt(s, 2));
            newPose.positionMap[name].y = std::stof(wordAt(s, 3));
        }
        else if (wordAt(s) == "(rotate)") {
            name = wordAt(s, 1);
            newPose.rotationMap[name] = std::stof(wordAt(s, 2));
        }
    }
    std::cout << "Success: Loaded in sequence from file `" << filename << "`" << std::endl;
    return true;
}

inline bool readFigureFromFile(Figure& figure, const char* filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Unable to open `" << filename << "`!" << std::endl;
        file.close();
        return false;
    }

    figure.clear();

    // (part) [PART NAME]
    // [BOUNDS X] [BOUNDS Y]
    // [BOUNDS WIDTH] [BOUNDS HEIGHT]
    // [PIVOT X] [PIVOT Y]
    // [PARENT] [NOTCH ATTACHED TO]
    // (notch) [NOTCH 1 X] [NOTCH 1 X]
    // (notch) [NOTCH 2 X] [NOTCH 2 X]
    // (notch) [NOTCH n X] [NOTCH n X]
    std::vector<std::string> parents{};
    std::string s{};
    while (getline(file, s)) {
        if (wordAt(s) == "(part)") {
            Part newPart{};

            try {
                newPart.name = wordAt(s, 1);
                getline(file, s);
                newPart.bounds.x = std::stoi(wordAt(s, 0));
                newPart.bounds.y = std::stoi(wordAt(s, 1));
                getline(file, s);
                newPart.bounds.width = std::stoi(wordAt(s, 0));
                newPart.bounds.height = std::stoi(wordAt(s, 1));
                getline(file, s);
                newPart.pivot.x = std::stoi(wordAt(s, 0));
                newPart.pivot.y = std::stoi(wordAt(s, 1));
                getline(file, s);
                parents.push_back(wordAt(s, 0));
                newPart.connectedNotch = std::stoi(wordAt(s, 1));
            
                getline(file, s);
                while (wordAt(s) == "(notch)") {
                    int x = std::stoi(wordAt(s, 1));
                    int y = std::stoi(wordAt(s, 2));
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
            figure[i].position.x = windowWidth/2.0f;
            figure[i].position.y = windowHeight/2.0f;
        }
    }
    figure.root = figure.getPartByName(figure.getPose().positionMap.begin()->first);

    file.close();
    return true;
}

inline bool exportAsSpritesheet(Texture texture, Sequence sequence) {
    RenderTexture target = LoadRenderTexture(windowWidth*sequence.size(), windowHeight);
    Figure figure{};

    bool texSuccess = 0 != target.texture.width;
    bool figSuccess = readFigureFromFile(figure, figureFilename);
    bool dirSuccess = 0 == MakeDirectory(outputDirectory);

    if (!texSuccess || !figSuccess || !dirSuccess) {
        if (!texSuccess) std::cerr << "Error: Failed to create a render texture (width was " << target.texture.width << ")" << std::endl;
        if (!figSuccess) std::cerr << "Error: Failed to read figure" << std::endl;
        if (!dirSuccess) std::cerr << "Error: Failed to create output directory, `" << outputDirectory << "`" << std::endl;
        UnloadRenderTexture(target);
        return false;
    }

    Camera2D camera{
        .offset = (Vector2){windowWidth/2.0f, windowHeight/2.0f},
        .target = (Vector2){windowWidth/2.0f, windowHeight/2.0f},
        .zoom = cameraZoomLevel,
    };

    for (int i = 0; i < sequence.size(); i++) {
        figure.setPose(sequence[i]);
        figure.update();
        BeginTextureMode(target); {
            BeginMode2D(camera); {
                figure.draw(texture, WHITE);
            }
            EndMode2D();
        }
        EndTextureMode();
        camera.offset.x += windowWidth;
    }

    Image output = LoadImageFromTexture(target.texture);
    ImageFlipVertical(&output); // For some reason, the image loads in flipped

    bool exportSuccess = ExportImage(output, TextFormat("%s/%s-%s.png", outputDirectory, textureFilename, sequenceFilename));

    if (!exportSuccess)
        std::cerr << "Error: Failed to export image" << std::endl;
    UnloadRenderTexture(target);
    UnloadImage(output);
    return exportSuccess ? true : false;
}
