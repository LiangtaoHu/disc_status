#include <iostream>
#include <fstream>
#include <string>
#include <vector>

enum Activity_Type {
    Playing,
    Streaming,
    Listening,
    Watching,
    CustomStatus,
    Competing,
    HangStatus
};

struct struct_activity {
    Activity_Type type;
    std::string state;
    std::string details;
    std::string name;
    struct {
        std::string large_image;
        std::string hover_over_text;
        std::string image_link;
    } struct_assets;
    std::string to_string() {
        return  "\tActivityType: " + std::to_string(type) + 
                "\n\tState: " + state + 
                "\n\tDetails: " + details + 
                "\n\tName: " + name + 
                "\n\tAssets: " + 
                    "\n\t\tLarge Image: " + struct_assets.large_image +
                    "\n\t\tHover-over Text: " + struct_assets.hover_over_text +
                    "\n\t\tImage Link: " + struct_assets.image_link + "\n"; 
    }
};

// enum offsets {
//     ACTIVITY_TYPE_OFFSET = sizeof(char)*sizeof("ActivityType:"),
//     STATE_OFFSET = sizeof(char)*sizeof("State:"),
//     DETAILS_OFFSET = sizeof(char)*sizeof("Details:"),
//     NAME_OFFSET = sizeof(char)*sizeof("Name:"),
//     LARGE_IMAGE_OFFSET = sizeof(char)*sizeof("\tLarge Image:"),
//     HOVER_OVER_TEXT_OFFSET = sizeof(char)*sizeof("\tHover-over Text:"),
//     IMAGE_LINK_OFFSET = sizeof(char)*sizeof("\tImage Link:")
// };

void load_saved_config(struct_activity* activity) {
    std::ifstream config_txt("config.txt"); // Simplification?
    if (config_txt.is_open()) {
        std::string line;

        std::getline(config_txt, line);
        activity->type = Activity_Type(std::stoi(line.substr(line.find(":") + 2)));

        std::getline(config_txt, line);
        activity->state = line.substr(line.find(":") + 2);

        std::getline(config_txt, line);
        activity->details = line.substr(line.find(":") + 2);

        std::getline(config_txt, line);
        activity->name = line.substr(line.find(":") + 2);

        std::getline(config_txt, line);

        std::getline(config_txt, line);
        activity->struct_assets.large_image = line.substr(line.find(":") + 2);

        std::getline(config_txt, line);
        activity->struct_assets.hover_over_text = line.substr(line.find(":") + 2);

        std::getline(config_txt, line);
        activity->struct_assets.image_link = line.substr(line.find(":") + 2);
    }
}

void startup(struct_activity* activity) {
    // Check current directory to see if there's a config.txt & presets.txt 
    std::ifstream read_preset_file("presets.txt");
    if (read_preset_file.is_open() != true) {
        std::ofstream write_preset_file("presets.txt");
        write_preset_file << "0";
        write_preset_file.close();
    }
    read_preset_file.close();

    std::ifstream read_config_file("config.txt");
    if (read_config_file.is_open()) {
        // load_saved_config(activity);

    } else {
        std::ofstream write_config_file("config.txt");
        write_config_file.close();
    }
}

// void save_activity(struct_activity* activity) {
//     int new_preset_num = 1;
//     std::string current_num;

//     std::ifstream read_preset_file("presets.txt");
//     if (read_preset_file.is_open() && std::getline(read_preset_file, current_num)) {
//         if (!current_num.empty()) {
//             new_preset_num = std::stoi(current_num) + 1;
//         }
//     }
//     read_preset_file.close();

//     std::ofstream write_preset_file("presets.txt", std::ios::app);
//     write_preset_file << "\nPreset #" << new_preset_num << "\n" << activity->to_string();
//     write_preset_file.close();
// }

void save_activity(struct_activity* activity) {
    std::vector<std::string> lines;
    std::string line;
    int current_count = 0;

    // 1. Read existing content
    std::ifstream read_file("presets.txt");
    if (read_file.is_open()) {
        if (std::getline(read_file, line) && !line.empty()) {
            current_count = std::stoi(line);
        }
        // Store the rest of the file
        while (std::getline(read_file, line)) {
            lines.push_back(line);
        }
        read_file.close();
    }

    int new_preset_num = current_count + 1;

    // 2. Overwrite the file from the start
    // Opening without std::ios::app clears the file
    std::ofstream write_file("presets.txt"); 
    if (write_file.is_open()) {
        // Write the NEW counter first
        write_file << new_preset_num << "\n";

        // Put back the old lines
        for (const auto& l : lines) {
            write_file << l << "\n";
        }

        // Append the new activity at the end
        write_file << "Preset #" << new_preset_num << "\n" << activity->to_string();
        write_file.close();
    }
}

int main() {
    // Create preset file
    // Functions that can delete, load, or save presets
    // Functions that can load a current config
    // -> Asks if you want to save current config after loading
    // Program should automatically load up last used config
    struct_activity* current_activity = new struct_activity();
    // startup(current_activity);
    // current_activity->type = Playing;
    // current_activity->state = "1 Player Entered.";
    // current_activity->details = "Dreaming on Derse.";
    // current_activity->name = "SBURB™ BETA";
    // current_activity->struct_assets.large_image = "https://imgur.com/4V4dkia.gif";
    // current_activity->struct_assets.hover_over_text = "SBURB™ BETA";
    // current_activity->struct_assets.image_link = "mpfsa.com";
    // save_activity(current_activity);
    // delete current_activity;
    load_saved_config(current_activity);
    std::cout << current_activity->to_string();
}