#pragma once

#include "DatabaseHelper.h"

class WifiHelper {
  DatabaseHelper &databaseHelper;

 public:
  explicit WifiHelper(DatabaseHelper &databaseHelper);

  void setup();
};
