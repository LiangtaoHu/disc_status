#define DISCORDPP_IMPLEMENTATION
#include "discordpp.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <string>
#include <functional>
#include <csignal>
#include <chrono>

class User {
    private:
        uint64_t m_application_id;
        std::shared_ptr<discordpp::Client> m_client;
        std::string m_refreshToken;
        discordpp::Activity m_currentActivity;

        std::thread m_token_thread;
        std::atomic<bool> m_running = false;
        std::atomic<uint32_t> m_expire_time = 0;

    public:
        User(uint64_t app_id) {
            m_application_id = app_id;
            m_client = std::make_shared<discordpp::Client>();
            oauth2_process();
        }

        ~User() {
            if (m_running) {
                m_running = false;
                if (m_token_thread.joinable()) {
                    m_token_thread.join();
                }
            }
        }

        // Oauth2 Process
        // One Function that prompts user to authorize then handles auth code to get tokens.
        // - Auth process first requires AuthorizationArgs: ClientID(?), Scopes [DefaultPresenceScopes], State(?)*, Nonce(?)*, CodeChallenge*, IntegrationType(?)*
        // - ClientID (Disc Application ID) is provided here or takes the default value of Client::SetApplicationID
        // - State: Used to verify request, hashing, etc. I feel like this is extra credit for now
        // - Nonce: Ditto
        // - CodeChallenge: Optional, but needed for us since we want to exchange for tokens, use CreateAuthorizationCodeVerifier for challenge value to pass
        // - IntegrationType: N/A, have not researched.
        // 
        // Afterwards we do the Client::GetToken with the verifier from CreateAuthorizationCodeVerifier and authcode, applicationID, redirectURI*, callback

        /* ------------------------------------------------------------------------------------------ */
        void oauth2_process() {
            auto codeVerifier = m_client->CreateAuthorizationCodeVerifier();
            discordpp::AuthorizationArgs args{};
            args.SetClientId(m_application_id);
            args.SetScopes(discordpp::Client::GetDefaultPresenceScopes());
            args.SetCodeChallenge(codeVerifier.Challenge());

            m_client->Authorize(args, [this, codeVerifier](discordpp::ClientResult result, std::string const code, std::string redirectUri) {
                if (result.Successful()) {
                    std::cout << "Auth Code Received." << std::endl;
                    m_client->GetToken(m_application_id, code, codeVerifier.Verifier(), redirectUri, [this](
                        discordpp::ClientResult result, 
                        std::string accessToken, 
                        std::string refreshToken, 
                        discordpp::AuthorizationTokenType tokenType, 
                        int32_t expiresIn, 
                        std::string scopes) 
                        {
                            std::cout << "Access Token Received." << std::endl;
                            m_expire_time = expiresIn;
                            m_running = true;
                            m_refreshToken = refreshToken;
                            m_client->UpdateToken(tokenType, accessToken, [this](discordpp::ClientResult result) {
                                if (result.Successful()) {
                                    std::cout << "Updated Access Token Successfully." << std::endl;
                                    m_client->Connect();
                                    start_background_loop();
                                } else {
                                    std::cerr << "Failed to update access token: " << result.Error() << std::endl;
                                }
                            });
                        });
                } else {
                    std::cerr << "Auth Error: " << result.Error() << std::endl;
                }
            });
        }
        void refresh_tokens() {
            m_client->RefreshToken(m_application_id, m_refreshToken, [this](
                discordpp::ClientResult result, 
                std::string accessToken,
                std::string refreshToken, 
                discordpp::AuthorizationTokenType tokenType, 
                int32_t expiresIn, 
                std::string scopes) {
                    if (result.Successful()) {
                        m_refreshToken = refreshToken;
                        m_expire_time = expiresIn;
                        m_client->UpdateToken(tokenType, accessToken, [this](discordpp::ClientResult result) {
                                    if (result.Successful()) {
                                        std::cout << "Updated Access Token Successfully." << std::endl;
                                        m_client->Connect();
                                    } else {
                                        std::cerr << "Failed to update access token: " << result.Error() << std::endl;
                                    }
                                });
                    } else {
                        std::cerr << "Failed to get new tokens: " << result.Error() << std::endl;
                    }

                });
        }
        // Custom Activities

        // Upon a successful client, gives the ability to pass in activities and set them as your custom activity

        // (?) - Optional
        // Activity has the following information: Name, Type*, State(?), StateUrl(?), Details(?), DetailsURL(?), ApplicationID(?), Assets*, Timestamps*(?), ActivityParty*(?), Secrets*(?)
        // Type can be the following: Playing, Streaming, Listening, Watching, CustomStatus, Competing, HangStatus
        // Assets has the following information: LargeImage(?), LargeText(?), LargeURL(?), SmallImage(?), SmallText(?), SmallURL(?)
        // Timestamps has the following information: Start [milliseconds since Unix Epoch], End [milliseconds since Unix Epoch] 
        //      - Do one or the other here.
        // ActivityParty has the following information: Id, CurrentSize, MaxSize
        // Secrets - Join 
        void create_secret(discordpp::ActivitySecrets* secrets, std::string secret) {
            secrets->SetJoin(secret);
        }

        void create_party(discordpp::ActivityParty* party, int32_t current_size, int32_t max_size, std::string id) {
            party->SetCurrentSize(current_size);
            party->SetId(id);
            party->SetMaxSize(max_size);
        }

        void create_timestamp(discordpp::ActivityTimestamps* timestamp, uint64_t milliseconds, bool start) {
            if (start) {
                timestamp->SetStart(milliseconds);
            } else {
                timestamp->SetEnd(milliseconds);
            }
        }

        void create_assets(discordpp::ActivityAssets* assets, 
        std::optional<std::string> large_image = std::nullopt, 
            std::optional<std::string> large_text = std::nullopt, 
            std::optional<std::string> large_url = std::nullopt, 
            std::optional<std::string> small_image = std::nullopt, 
            std::optional<std::string> small_text = std::nullopt, 
            std::optional<std::string> small_url = std::nullopt) 
            {
                assets->SetLargeImage(large_image);
                assets->SetLargeText(large_text);
                assets->SetLargeUrl(large_url);
                assets->SetSmallImage(small_image);
                assets->SetSmallText(small_text);
                assets->SetSmallUrl(small_url);
        }
        // Function to create activity 
        void update_activity(
            std::string Name, 
            discordpp::ActivityTypes type, 
            std::optional<std::string> state = std::nullopt, 
            std::optional<std::string> stateUrl = std::nullopt, 
            std::optional<std::string> details = std::nullopt, 
            std::optional<std::string> detailsURL = std::nullopt, 
            std::optional<discordpp::ActivityAssets> assets = std::nullopt,
            std::optional<discordpp::ActivityTimestamps> timestamp = std::nullopt,
            std::optional<discordpp::ActivityParty> party = std::nullopt,
            std::optional<discordpp::ActivitySecrets> secret = std::nullopt
            ) {
                m_currentActivity.SetName(Name);
                m_currentActivity.SetType(type);
                m_currentActivity.SetState(state);
                m_currentActivity.SetStateUrl(stateUrl);
                m_currentActivity.SetDetails(details);
                m_currentActivity.SetDetailsUrl(detailsURL);
                if (assets)    m_currentActivity.SetAssets(*assets);
                if (timestamp) m_currentActivity.SetTimestamps(*timestamp);
                if (party)     m_currentActivity.SetParty(*party);
                if (secret)    m_currentActivity.SetSecrets(*secret);
                set_activity();
            }
    private:
        // Function to set activity 
        void set_activity() {
            m_client->UpdateRichPresence(m_currentActivity, [](discordpp::ClientResult result) {
                if (result.Successful()) {
                    std::cout << "Changed Rich Presence Successfully" << std::endl;
                } else {
                    std::cout << "Failed to change Rich Presence: " << result.Error() << std::endl;
                }
            });
        }
        void start_background_loop() {
            if (m_running) {
                return;
            }
            m_running = true;
            m_token_thread = std::thread([this]() {
                while (m_running) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    if (m_expire_time > 0) {
                        m_expire_time--;
                    }
                    if (m_expire_time <= 300) {
                        std::cout << "Refreshing token" << std::endl;
                        refresh_tokens();
                    }
                }
            });
        }
};

int main() {
    const uint64_t APPLICATION_ID = 1;
    User temp = User(APPLICATION_ID);

    discordpp::ActivityParty party;
    discordpp::ActivityAssets assets;
    discordpp::Activity activity;
    temp.create_party(&party, 1, 4, "1");
    temp.create_assets(&assets, "https://imgur.com/4V4dkia.gif", std::nullopt, std::nullopt, std::nullopt, std::nullopt, std::nullopt);
    temp.update_activity("SBURB™ BETA", discordpp::ActivityTypes::Playing, "1 Player Entered.", std::nullopt, "Dreaming on Derse.", std::nullopt, assets, std::nullopt, party, std::nullopt);
}