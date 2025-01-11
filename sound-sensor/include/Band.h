#pragma once

struct Band {
  int* bands;
  int count;
  int minValue;

  Band(int* bands, int count, int minValue);
};
