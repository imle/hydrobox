#include "FeedChart.h"


// https://gh.growgh.com/docs/Feedcharts/GH_FloraSeries-REC_03216am.pdf
const FeedChart DefaultFeedChart{
    .regimens = {
        FeedChartRegimen{ // 1 - Seedling
            .flora_micro = 2.5f / MillilitersPerUSGallon,
            .flora_gro = 2.5f / MillilitersPerUSGallon,
            .flora_bloom = 2.5f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 2 - Early Growth
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 10.0f / MillilitersPerUSGallon,
            .flora_bloom = 2.5f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 3 - Late Growth
            .flora_micro = 10.0f / MillilitersPerUSGallon,
            .flora_gro = 10.0f / MillilitersPerUSGallon,
            .flora_bloom = 5.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 4 - Transition
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 7.5f / MillilitersPerUSGallon,
            .flora_bloom = 7.5f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 5 - Early Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 2.5f / MillilitersPerUSGallon,
            .flora_bloom = 10.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 6 - Early Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 2.5f / MillilitersPerUSGallon,
            .flora_bloom = 10.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 7 - Mid Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 2.5f / MillilitersPerUSGallon,
            .flora_bloom = 12.5f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 8 - Mid Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 2.5f / MillilitersPerUSGallon,
            .flora_bloom = 12.5f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 9 - Mid Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 0.0f / MillilitersPerUSGallon,
            .flora_bloom = 15.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 10 - Late Bloom
            .flora_micro = 7.5f / MillilitersPerUSGallon,
            .flora_gro = 0.0f / MillilitersPerUSGallon,
            .flora_bloom = 15.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 11 - Ripen
            .flora_micro = 5.0f / MillilitersPerUSGallon,
            .flora_gro = 0.0f / MillilitersPerUSGallon,
            .flora_bloom = 15.0f / MillilitersPerUSGallon,
        },
        FeedChartRegimen{ // 12 - Flush
            .flora_micro = 0.0f / MillilitersPerUSGallon,
            .flora_gro = 0.0f / MillilitersPerUSGallon,
            .flora_bloom = 0.0f / MillilitersPerUSGallon,
        },
    }
};