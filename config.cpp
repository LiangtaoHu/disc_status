#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>

enum Activity_Type {
    Playing,
    Streaming,
    Listening,
    Watching,
    CustomStatus,
    Competing,
    HangStatus
};

std::unordered_map<std::string, Activity_Type> const activity_type_table = {
    {"Playing", Activity_Type::Playing},
    {"Streaming", Activity_Type::Streaming},
    {"Listening", Activity_Type::Listening},
    {"Watching", Activity_Type::Watching},
    {"CustomStatus", Activity_Type::CustomStatus},
    {"Competing", Activity_Type::Competing},
    {"HangStatus", Activity_Type::HangStatus},
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
        return  "ActivityType\t" + std::to_string(type) + 
                "\nState\t" + state + 
                "\nDetails\t" + details + 
                "\nName\t" + name + 
                "\nLarge Image\t" + struct_assets.large_image +
                "\nHover-over Text\t" + struct_assets.hover_over_text +
                "\nImage Link\t" + struct_assets.image_link + "\n"; 
    }
};




void load_config(struct_activity* activity) {
    // Read the preset file (guaranteed to exist) and load the values of preset #[config.txt value] into activity
    // Check if first line is a valid integer
    std::ofstream preset_file 
}

void load_presets(std::vector<struct_activity*> presets) {
    // Load all presets saved in presets.txt
    // Check if every line is valid
    std::ifstream preset_file("presets.txt");
    std::string curr_line;
    struct_activity* new_activity = nullptr;
    while (std::getline(preset_file, curr_line)) { // Until EOF or blank line
        if (curr_line == "{") {
            // Start creating an activity
            new_activity = new struct_activity;
        } else if (curr_line == "}") {
            // End of the activity, push into presets
            // Make new_activity a null vector again
            presets.push_back(new_activity);
            new_activity = nullptr;
        } else {
            // If nothing else, check if the current line refers to a valid attribute of an activity then
            // Format: Attribute Tab Value
            std::string delimiter = "\t";
            std::string attribute = curr_line.substr(0, curr_line.find(delimiter));
            std::string value = curr_line.substr(curr_line.find(delimiter));
            
            if (attribute == "ActivityType") {
                auto it = activity_type_table.find(value);
                if (it != activity_type_table.end()) {
                    new_activity->type = it->second;
                }
            } else {
                // All other attributes take string values so we don't need to find a way to do some conversion here
                // We still need to isolate which attribute it is though so we need a string to attribute lookup
                // If invalid attribute, skip line
            }
        }
    }
}

void startup(std::vector<struct_activity*> presets, struct_activity* activity) {
    if (!std::filesystem::exists("presets.txt")) {
        std::ofstream preset_file("presets.txt");
        preset_file << "0";
        preset_file.close();
    } else {
        load_presets(presets);
    }
    if (!std::filesystem::exists("config.txt")) {
        std::ofstream config_file("config.txt");
        config_file << "-1";
    } else {
        load_config(activity);
    }
}

void save_activity(struct_activity* activity) {
    // Append current activity into the global presets folder
    // Write activity into presets.txt 
    // Increment first line of presets.txt which represents the total presets so far


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

void delete_activity(int idx) {
    // Delete the idx-th element in our preset array
    // Write array into presets.txt
};

void use_preset(int idx) {
    // Using our prests array, we can change our pointer to activity to that instead
    // Apply changes afterwards
};

void apply_changes(struct_activity* activity) {
    // Push towards discord
}


int main() {
    // Create preset file
    // Functions that can delete, load, or save presets
    // Functions that can load a current config
    // -> Asks if you want to save current config after loading
    // Program should automatically load up last used config
    std::vector<struct_activity*> presets;
    struct_activity* current_activity;
    startup(presets, current_activity);





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