A more accurate Brave browsing savings predictor based on page features, ad-blocking behaviour and a ML model.

Features needed to extract for bandwidth saving prediction:

- adblockRequests
- metrics.firstMeaningfulPaint
- resources.script.requestCount
- resources.image.size
- resources.font.size
- resources.document.requestCount
- resources.document.size
- resources.stylesheet.requestCount
- resources.other.requestCount
- resources.other.size
- resources.third-party.requestCount
- dom.Total DOM Elements
- dom.Maximum DOM Depth
- diagnostics.numTasksOver25ms
- diagnostics.numTasksOver50ms
- diagnostics.numTasksOver500ms
- diagnostics.maxServerLatency
- bootupTime
- criticalRequestChains.length

For CPU execution time prediction:

adblockRequests                       0.277856
- bootupTime                            0.057019
- criticalRequestChains.duration        0.186089
- criticalRequestChains.length          0.111433
- criticalRequestChains.transferSize    0.076248
- diagnostics.maxRtt                    0.019787
- diagnostics.maxServerLatency          0.101962
- diagnostics.numTasks                  0.058361
- diagnostics.numTasksOver100ms         0.054212
- diagnostics.numTasksOver10ms          0.114040
- diagnostics.numTasksOver25ms          0.093246
- diagnostics.numTasksOver500ms         0.020577
- diagnostics.numTasksOver50ms          0.112827
diagnostics.rtt                       0.051335
diagnostics.throughput                0.040847
diagnostics.totalTaskTime             0.092413
- dom.Maximum Child Elements            0.057666
- dom.Maximum DOM Depth                 0.001496
- dom.Total DOM Elements                0.024522
metrics.firstCPUIdle                  0.073072
metrics.firstMeaningfulPaint          0.062487
metrics.interactive                   0.061118
metrics.observedDomContentLoaded      0.023359
metrics.observedFirstVisualChange     0.041779
metrics.observedLastVisualChange      0.126127
metrics.observedLoad                  0.143081
resources.document.requestCount       0.136539
resources.document.size               0.007332
resources.font.requestCount           0.162274
resources.font.size                   0.024121
resources.image.requestCount          0.032487
resources.image.size                  0.030823
resources.media.requestCount          0.006489
resources.media.size                  0.014783
resources.other.requestCount          0.083628
resources.other.size                  0.014151
resources.script.requestCount         0.100700
resources.script.size                 0.012147
resources.stylesheet.requestCount     0.030678
resources.stylesheet.size             0.069615
resources.third-party.requestCount    0.047229
resources.third-party.size            0.016127
resources.total.requestCount          0.049290
resources.total.size                  0.033648

In addition to those, the identifiers of specific third-party entities included among the blocked requests have a significant impact as well.

The data that needs to be combined into a single input for the model comes from different components within Chromium:
- adblocking info (requests and the third-parties that that can be computed from those) comes from Brave's adblocker, need to observe those with minimal changes to `brave_shields`
- resource type information is conveniently available in WebContentsObserver. Need to see how to compute critical request chains from that
- performance metrics - now ready via `Performance Observers`, one line change in Chromium to hook in Brave's custom set of performance observers
- DOM statistics - ?
- JS execution statistics (num tasks above certain duration) - got a proxy from renderer CPU time, but not the same
- max server latency - ?



