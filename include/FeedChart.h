#ifndef HPCC_FEEDCHART_H
#define HPCC_FEEDCHART_H

const float LitersPerUSGallon = 3.79f;
const float MillilitersPerUSGallon = LitersPerUSGallon * 1000.0f;

struct FeedChartRegimen {
  double flora_micro;
  double flora_gro;
  double flora_bloom;
};

const int FeedChartRegimens = 12;
struct FeedChart {
  FeedChartRegimen regimens[FeedChartRegimens];
};

extern const FeedChart DefaultFeedChart;

#endif //HPCC_FEEDCHART_H
