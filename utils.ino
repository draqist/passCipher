int user_number = 3;
char* lcase = "abcdefghijklmnopqrstuvwxyz";
char* ucase = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
char* symbols = "!$%^@#*.:;,?-_<>=";

bool save_encode_credential(fs::FS &fs, const char * path, char * key, const char * password, const char * email, const char * username ) {
  char fullkey[17];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);

  struct storage user;
  encrypt(password, fullkey, user.password);
  strcpy(user.email, email);
  strcpy(user.username, username);
  user.style = 1;

  

  File file = fs.open(path, FILE_WRITE);

  if (!file) {
    Serial.println("- failed to open file for writing");
    return false;

  }

  file.write((const uint8_t *)&user, sizeof(user));
  file.close();
  Serial.println("Saved ");
  return true;
}

bool save_user_credential(const char * website, const char * password, const char * email, const char * username ) {
  char fullkey[17];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);
  
  char str[32];
  char buf[4];
  
  sprintf(buf, "%d", current_user_number);
  strcpy(str, "/");
  strcat(str, buf);
  strcat(str, "/");
  strcat(str, website);
  strcat(str, ".txt");
  
  Serial.println(fullkey);
  
  return save_encode_credential(SPIFFS, str, fullkey, password, email, username );
}



bool read_decode_credential(fs::FS &fs, const char * path,  char * key ) {
  char fullkey[17];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);

  memset( decipheredTextOutput, 0, sizeof( decipheredTextOutput ) );
  memset( user_out.password, 0, sizeof( user_out.password ) );
  memset( user_out.email, 0, sizeof( user_out.email ) );

  struct storage user;

  //read from memory
  File file = fs.open(path, FILE_READ);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return false;

  }
  file.read((uint8_t *)&user, sizeof(user));
  file.close();


  decrypt(user.password, fullkey, decipheredTextOutput);
  strcpy(user_out.email, user.email);

  Serial.println("\n\nDeciphered text:");
  for (int i = 0; i < 64; i++) {
    user_out.password[i] = (char)decipheredTextOutput[i];
    //    Serial.print((char)decipheredTextOutput[i]);
  }
  Serial.print(user_out.password);

  return true;
}


bool read_user_credential(const char * website ) {
  char fullkey[17];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);
  
  char str[32];
  char buf[4];
  Serial.println(fullkey);
  
  sprintf(buf, "%d", current_user_number);
  strcpy(str, "/");
  strcat(str, buf);
  strcat(str, "/");
  strcat(str, website);
  strcat(str, ".txt");
  Serial.println(str);
  return read_decode_credential(SPIFFS, str, fullkey );
     
}


bool create_user(fs::FS &fs, const char * path, char * key) {
  char fullkey[17];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);
  Serial.print("fullkey = ");
  Serial.println(fullkey);

  memset( cipherTextOutput, 0, sizeof( cipherTextOutput ) );
  encrypt("done", fullkey, cipherTextOutput);


  //  for (int i = 0; i < 64; i++) {
  //    Serial.print((char)decipheredTextOutput[i]);
  //  }

  File file = fs.open(path, FILE_WRITE);

  if (!file) {
    Serial.println("- failed to open file for writing");
    return false;
  }


  file.write((const uint8_t *)cipherTextOutput, sizeof(cipherTextOutput));
  file.close();
  Serial.println(" Done Writing ");
  return true;

}

bool read_create_user(fs::FS &fs, const char * path, char * key) {
  char fullkey[17];
  char done[4];
  strcpy(fullkey, key);
  strcat(fullkey, key);
  strncat(fullkey, key, 4);

  memset( cipherTextOutput, 0, sizeof( cipherTextOutput ) );
  memset( decipheredTextOutput, 0, sizeof( decipheredTextOutput ) );
  encrypt("done", fullkey, cipherTextOutput);

  //read from memory
  File file = fs.open(path, FILE_READ);
  if (!file) {
    Serial.println("- failed to open file for writing");
    return false;

  }
  file.read((uint8_t *)&cipherTextOutput, sizeof(cipherTextOutput));
  file.close();


  decrypt(cipherTextOutput, fullkey, decipheredTextOutput);

  if ((char)decipheredTextOutput[0] == 'd' && (char)decipheredTextOutput[1] == 'o' && (char)decipheredTextOutput[2] == 'n' && (char)decipheredTextOutput[3] == 'e' ) {
    //    Serial.println("True User detected");
    return true;
  }

  return false;

}

bool randomString() {
  memset( password_buf, 0, sizeof( password_buf ) );
  int i = 0;

  while (i < 4) {
    password_buf[i] = ucase[random(0, strlen(ucase) - 1)];
    i++;
  }
  while (i < 8) {
    password_buf[i] = symbols[random(0, strlen(symbols) - 1)];
    i++;
  }
  while (i < 12) {
    sprintf(&password_buf[i], "%d", random(0, 9));
    i++;
  }
  while (i < 16) {
    password_buf[i] = lcase[random(0, strlen(lcase) - 1)];
    i++;
  }
  password_buf[16] = '\0';
  return true;
}

int getUserNumber(char * key) {
  int user = -1;
  char str[4] = "/";
  for (int i = 0; i < user_number; i++) {
    sprintf(&str[1], "%d", i);
    if (read_create_user(SPIFFS, str, key )) {
      user = i;
      break;
    }
  }
  return user;
}
