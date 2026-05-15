
// Oauth2 Process

// User struct which holds tokens

// One Function that prompts user to authorize then handles auth code to get tokens.

/* ------------------------------------------------------------------------------------------ */

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

// Activity information in the form of a struct/class/etc. [Part of Overrall User struct]

// Function to create activity 

// Function to set activity 

/* ------------------------------------------------------------------------------------------ */

// Main Loop

// Function to refresh tokens