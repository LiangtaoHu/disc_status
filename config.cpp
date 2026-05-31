// TODO: Some edge cases in load_prefixes
// TODO: Put into a class for better management

#include <iostream>
#include <fstream>
#include <filesystem>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>

enum Activity_Type {
    Playing,
    Streaming,
    Listening,
    Watching,
    CustomStatus,
    Competing,
    HangStatus
};

static const std::unordered_map<std::string, Activity_Type> activity_type_table = {
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
    struct struct_assets_type{
        std::string large_image;
        std::string hover_over_text;
        std::string image_link;
    } struct_assets;
    std::string to_string() {
        static const std::string activity_names[] = {
            "Playing", "Streaming", "Listening", "Watching", 
            "CustomStatus", "Competing", "HangStatus"
        };
        
        std::string type_string = activity_names[type];
        return  "{\nActivityType\t" + type_string + 
                "\nState\t" + state + 
                "\nDetails\t" + details + 
                "\nName\t" + name + 
                "\nLarge Image\t" + struct_assets.large_image +
                "\nHover-over Text\t" + struct_assets.hover_over_text +
                "\nImage Link\t" + struct_assets.image_link + "\n}\n"; 
    }
};

static const std::unordered_map<std::string, std::string struct_activity::*> string_attribute_table = {
    {"State", &struct_activity::state},
    {"Details", &struct_activity::details},
    {"Name", &struct_activity::name}
};

static const std::unordered_map<std::string, std::string struct_activity::struct_assets_type::*> asset_attribute_table = {
    {"Large Image", &struct_activity::struct_assets_type::large_image},
    {"Hover-over Text", &struct_activity::struct_assets_type::hover_over_text},
    {"Image Link", &struct_activity::struct_assets_type::image_link}
};

void load_config(std::vector<struct_activity*>& presets, struct_activity*& activity) {
    // Read the preset file (guaranteed to exist) and load the values of preset #[config.txt value] into activity
    // Check if first line is a valid integer
    std::ifstream config_file("config.txt");
    std::string curr_line;
    std::getline(config_file, curr_line);
    config_file.close();

    if (curr_line.empty()) {
        // Valid file but it's empty, need to rewrite
        std::ofstream w_config_file("config.txt");
        w_config_file << "-1";
        w_config_file.close();
    } else {
        try {
            // Convert entire string to int if possible
            size_t processed_chars = 0;
            int preset_num = std::stoi(curr_line, &processed_chars);
            // Compare against presets length and check if its positive to see if its valid
            if (processed_chars < curr_line.length()) {
                throw std::invalid_argument("");
            } else if (preset_num >= 0 && preset_num < presets.size()) {
                activity = presets[preset_num];
            } else {
                throw std::invalid_argument("");
            }
        }
        catch (const std::invalid_argument& e) {
            std::ofstream w_config_file("config.txt");
            w_config_file << "-1";
            w_config_file.close();
        }
        catch (const std::out_of_range& e) {
            std::ofstream w_config_file("config.txt");
            w_config_file << "-1";
            w_config_file.close();
        }
    }
}

void load_presets(std::vector<struct_activity*>& presets) {
    // Load all presets saved in presets.txt
    // Check if every line is valid
    std::ifstream preset_file("presets.txt");
    std::string curr_line;
    struct_activity* new_activity = nullptr;
    while (std::getline(preset_file, curr_line)) { // Until EOF or blank line
        if (curr_line == "{") {
            // Start creating an activity
            new_activity = new struct_activity();
        } else if (curr_line == "}") {
            // End of the activity, push into presets
            // Make new_activity a null vector again
            if (new_activity != nullptr) {
                presets.push_back(new_activity);
                new_activity = nullptr;
            }
        } else {
            // If nothing else, check if the current line refers to a valid attribute of an activity then
            // Format: Attribute Tab Value
            std::string delimiter = "\t";
            size_t delim_pos = curr_line.find(delimiter);
            if (delim_pos == std::string::npos || new_activity == nullptr) { // Random line with no actual attribute or in an actual preset
                continue;
            }
            std::string attribute = curr_line.substr(0, delim_pos);
            std::string value = curr_line.substr(delim_pos + delimiter.length());
            
            if (attribute == "ActivityType") {
                auto it = activity_type_table.find(value);
                if (it != activity_type_table.end()) {
                    new_activity->type = it->second;
                }
            } else {
                auto string_it = string_attribute_table.find(attribute);

                // If found, update the member directly
                if (string_it != string_attribute_table.end()) {
                    auto member_ptr = string_it->second;
                    new_activity->*member_ptr = value;
                } else {
                    auto asset_it = asset_attribute_table.find(attribute);
                    if (asset_it != asset_attribute_table.end()) {
                        auto asset_ptr = asset_it->second;
                        new_activity->struct_assets.*asset_ptr = value;
                    }
                }
            }
        }
    }
}

void startup(std::vector<struct_activity*>& presets, struct_activity*& activity) {
    if (!std::filesystem::exists("presets.txt")) {
        std::ofstream preset_file("presets.txt");
        preset_file << "0\n";
        preset_file.close();
    } else {
        load_presets(presets);
    }
    if (!std::filesystem::exists("config.txt")) {
        std::ofstream config_file("config.txt");
        config_file << "-1";
        config_file.close();
    } else {
        load_config(presets, activity);
    }
}

void save_activity(std::vector<struct_activity*>& presets, struct_activity* activity) {
    std::ofstream write_file("presets.txt", std::ios::app); 
    if (write_file.is_open()) {
        write_file << activity->to_string();
        write_file.close();
        presets.push_back(activity);
    }

}

void delete_activity(std::vector<struct_activity*>& presets, int idx) {
    // Delete the idx-th element in our preset array
    // Write array into presets.txt
    if (idx >= 0 && idx < presets.size()) {
        delete presets[idx];
        presets.erase(presets.begin() + idx);
        std::ofstream preset_file("presets.txt");
        for (auto p : presets) {
            preset_file << p->to_string();
        }
        preset_file.close();
    }
};

void use_preset(std::vector<struct_activity*>& presets, int idx, struct_activity* activity) {
    // Using our prests array, we can change our pointer to activity to that instead
    // Apply changes afterwards
    if (idx >= 0 && idx < presets.size() && activity != nullptr) {
        *activity = *presets[idx];
        apply_changes(activity);
    }
};

void cleanup(std::vector<struct_activity*>& presets) {
    for (auto p : presets) {
        delete p;
    }
    presets.clear();
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
    // load_saved_config(current_activity);
    // std::cout << current_activity->to_string();
}