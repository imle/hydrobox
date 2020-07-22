#ifndef HPCC_FEEDCHART_H
#define HPCC_FEEDCHART_H

const double LitersPerUSGallon = 3.79;
const double MillilitersPerUSGallon = LitersPerUSGallon * 1000.0;

struct FeedChartRegimen {
  double flora_micro;
  double flora_gro;
  double flora_bloom;
};

const int FeedChartRegimens = 12;
struct FeedChart {
  FeedChartRegimen regimens[FeedChartRegimens];
};

FeedChart const *DefaultFeedChart();

#endif //HPCC_FEEDCHART_H
