import numpy as np
import matplotlib.pyplot as plt

# Sample text data containing normal distribution values
text_data = """
Normal distribution: 109.654
Normal distribution: 89.2498
Normal distribution: 101.828
Normal distribution: 79.7286
Normal distribution: 98.5722
Normal distribution: 100.456
Normal distribution: 98.2769
Normal distribution: 115.424
Normal distribution: 98.1025
Normal distribution: 103.467
Normal distribution: 100.893
Normal distribution: 86.9417
Normal distribution: 91.0099
Normal distribution: 100.792
Normal distribution: 87.1457
Normal distribution: 117.407
Normal distribution: 99.9948
Normal distribution: 100.986
Normal distribution: 96.4754
Normal distribution: 102.672
Normal distribution: 102.872
Normal distribution: 116.27
Normal distribution: 97.5722
Normal distribution: 116.645
Normal distribution: 105.974
Normal distribution: 91.7497
Normal distribution: 109.129
Normal distribution: 104.278
Normal distribution: 89.5095
Normal distribution: 112.099
Normal distribution: 92.2248
Normal distribution: 91.4398
Normal distribution: 91.2188
Normal distribution: 104.73
Normal distribution: 96.6508
Normal distribution: 98.3493
Normal distribution: 93.1707
Normal distribution: 93.2368
Normal distribution: 98.1286
Normal distribution: 80.2996
Normal distribution: 117.506
Normal distribution: 117.537
Normal distribution: 99.5019
Normal distribution: 97.5473
Normal distribution: 108.325
Normal distribution: 122.842
Normal distribution: 96.2112
Normal distribution: 102.824
Normal distribution: 102.54
Normal distribution: 90.6297
Normal distribution: 128.222
Normal distribution: 106.711
Normal distribution: 106.713
Normal distribution: 117.504
Normal distribution: 97.3414
Normal distribution: 107.459
Normal distribution: 97.9807
Normal distribution: 114.346
Normal distribution: 119.94
Normal distribution: 95.884
Normal distribution: 81.3668
Normal distribution: 101.137
Normal distribution: 104.096
Normal distribution: 93.8639
Normal distribution: 115.21
Normal distribution: 101.957
Normal distribution: 112.474
Normal distribution: 85.3412
Normal distribution: 76.9463
Normal distribution: 99.5809
Normal distribution: 92.8572
Normal distribution: 102.996
Normal distribution: 118.79
Normal distribution: 94.6196
Normal distribution: 99.15
Normal distribution: 110.804
Normal distribution: 95.565
Normal distribution: 104.269
Normal distribution: 113.234
Normal distribution: 125.732
Normal distribution: 96.047
Normal distribution: 95.6318
Normal distribution: 113.605
Normal distribution: 106.718
Normal distribution: 103.135
Normal distribution: 106.746
Normal distribution: 100.333
Normal distribution: 93.8292
Normal distribution: 100.379
Normal distribution: 102.274
Normal distribution: 112.95
Normal distribution: 99.6574
Normal distribution: 111.15
Normal distribution: 116.141
Normal distribution: 104.161
Normal distribution: 97.6584
Normal distribution: 82.5
Normal distribution: 90.4675
Normal distribution: 104.557
Normal distribution: 103.922
Normal distribution: 102.99
Normal distribution: 98.4998
Normal distribution: 106.211
Normal distribution: 90.7117
Normal distribution: 104.413
Normal distribution: 102.486
Normal distribution: 107.342
Normal distribution: 91.6384
Normal distribution: 97.5975
Normal distribution: 87.3347
Normal distribution: 96.9132
Normal distribution: 90.4685
Normal distribution: 107.949
Normal distribution: 112.144
Normal distribution: 90.5073
Normal distribution: 90.3515
Normal distribution: 80.4639
Normal distribution: 99.4526
Normal distribution: 108.865
Normal distribution: 110.432
Normal distribution: 117.79
Normal distribution: 110.691
Normal distribution: 112.402
Normal distribution: 96.9564
Normal distribution: 95.5701
Normal distribution: 115.84
Normal distribution: 94.9079
Normal distribution: 94.4731
Normal distribution: 116.708
Normal distribution: 119.657
Normal distribution: 104.612
Normal distribution: 99.4865
Normal distribution: 107.54
Normal distribution: 89.4665
Normal distribution: 95.0571
Normal distribution: 119.063
Normal distribution: 98.4749
Normal distribution: 107.487
Normal distribution: 99.5749
Normal distribution: 109.383
Normal distribution: 108.786
Normal distribution: 87.0418
Normal distribution: 78.6443
Normal distribution: 93.9058
Normal distribution: 100.911
Normal distribution: 91.1979
Normal distribution: 109.393
Normal distribution: 83.9903
Normal distribution: 113.339
Normal distribution: 98.6572
Normal distribution: 107.474
Normal distribution: 94.3948
Normal distribution: 113.903
Normal distribution: 108.604
Normal distribution: 101.999
Normal distribution: 106.29
Normal distribution: 95.5781
Normal distribution: 120.688
Normal distribution: 96.4556
Normal distribution: 126.615
Normal distribution: 118.076
Normal distribution: 104.528
Normal distribution: 101.724
Normal distribution: 91.9473
Normal distribution: 79.7919
Normal distribution: 104.354
Normal distribution: 96.0435
Normal distribution: 98.6507
Normal distribution: 94.998
Normal distribution: 90.7182
Normal distribution: 83.8313
Normal distribution: 96.3454
Normal distribution: 111.737
Normal distribution: 105.256
Normal distribution: 104.565
Normal distribution: 98.4196
Normal distribution: 129.107
Normal distribution: 94.5568
Normal distribution: 101.768
Normal distribution: 88.7073
Normal distribution: 99.2116
Normal distribution: 95.4582
Normal distribution: 111.551
Normal distribution: 82.9272
Normal distribution: 99.3871
Normal distribution: 113.877
Normal distribution: 102.735
Normal distribution: 92.2991
Normal distribution: 100.669
Normal distribution: 89.9996
Normal distribution: 110.543
Normal distribution: 92.372
Normal distribution: 115.74
Normal distribution: 97.7591
Normal distribution: 109.901
Normal distribution: 101.548
Normal distribution: 95.9924
Normal distribution: 109.202
Normal distribution: 87.7488
Normal distribution: 107.876
Normal distribution: 107.304
Normal distribution: 110.316
Normal distribution: 94.9759
Normal distribution: 92.5144
Normal distribution: 106.43
Normal distribution: 98.0566
Normal distribution: 124.95
Normal distribution: 97.8303
Normal distribution: 105.946
Normal distribution: 107.603
Normal distribution: 94.5402
Normal distribution: 106.897
Normal distribution: 100.879
Normal distribution: 92.3485
Normal distribution: 98.0922
Normal distribution: 102.335
Normal distribution: 90.38
Normal distribution: 96.2568
Normal distribution: 98.7482
Normal distribution: 101.217
Normal distribution: 91.9061
Normal distribution: 112.447
Normal distribution: 91.1371
Normal distribution: 85.0224
Normal distribution: 109.732
Normal distribution: 111.556
Normal distribution: 81.6927
Normal distribution: 88.772
Normal distribution: 87.6436
Normal distribution: 114.222
Normal distribution: 105.981
Normal distribution: 111.712
Normal distribution: 96.8965
Normal distribution: 110.025
Normal distribution: 99.499
Normal distribution: 91.3085
Normal distribution: 109.025
Normal distribution: 103.929
Normal distribution: 110.748
Normal distribution: 102.186
Normal distribution: 86.9077
Normal distribution: 91.3473
Normal distribution: 101.463
Normal distribution: 112.497
Normal distribution: 94.6992
Normal distribution: 85.7447
Normal distribution: 91.219
Normal distribution: 104.784
Normal distribution: 91.5424
Normal distribution: 99.6696
Normal distribution: 98.776
Normal distribution: 104.905
Normal distribution: 106.252
Normal distribution: 101.059
Normal distribution: 98.0049
Normal distribution: 102.304
Normal distribution: 111.855
Normal distribution: 103.694
Normal distribution: 90.5235
Normal distribution: 109.929
Normal distribution: 111.874
Normal distribution: 101.152
Normal distribution: 103.779
Normal distribution: 104.082
Normal distribution: 90.771
Normal distribution: 105.212
Normal distribution: 117.158
Normal distribution: 90.8095
Normal distribution: 99.3717
Normal distribution: 112.416
Normal distribution: 96.5731
Normal distribution: 97.3896
Normal distribution: 99.941
Normal distribution: 95.5257
Normal distribution: 118.224
Normal distribution: 96.3739
Normal distribution: 101.989
Normal distribution: 98.5952
Normal distribution: 87.2214
Normal distribution: 98.3284
Normal distribution: 96.9247
Normal distribution: 107.019
Normal distribution: 102.394
Normal distribution: 91.0718
Normal distribution: 119.75
Normal distribution: 103.274
Normal distribution: 88.5501
Normal distribution: 106.585
Normal distribution: 88.2506
Normal distribution: 86.3532
Normal distribution: 120.145
Normal distribution: 101.154
Normal distribution: 97.5174
Normal distribution: 97.6745
Normal distribution: 104.263
Normal distribution: 96.3381
Normal distribution: 110.736
Normal distribution: 102.232
Normal distribution: 106.862
Normal distribution: 98.1313
Normal distribution: 98.8814
Normal distribution: 88.3539
Normal distribution: 81.0724
Normal distribution: 92.2662
Normal distribution: 98.1033
Normal distribution: 89.9138
Normal distribution: 100.114
Normal distribution: 86.9157
Normal distribution: 105.28
Normal distribution: 126.045
Normal distribution: 106.686
Normal distribution: 105.476
Normal distribution: 102.657
Normal distribution: 101.913
Normal distribution: 97.6405
Normal distribution: 86.1715
Normal distribution: 105.744
Normal distribution: 100.447
Normal distribution: 93.1157
Normal distribution: 90.561
Normal distribution: 104.43
Normal distribution: 92.6269
Normal distribution: 101.785
Normal distribution: 108.433
Normal distribution: 73.8452
Normal distribution: 107.011
Normal distribution: 102.113
Normal distribution: 109.815
Normal distribution: 100.475
Normal distribution: 97.4631
Normal distribution: 89.3203
Normal distribution: 102.037
Normal distribution: 98.9143
Normal distribution: 92.8925
Normal distribution: 101.734
Normal distribution: 87.5774
Normal distribution: 96.144
Normal distribution: 105.602
Normal distribution: 115.642
Normal distribution: 104.715
Normal distribution: 102.518
Normal distribution: 105.294
Normal distribution: 106.502
Normal distribution: 96.8091
Normal distribution: 104.119
Normal distribution: 112.069
Normal distribution: 92.8564
Normal distribution: 109.919
Normal distribution: 95.8647
Normal distribution: 99.4418
Normal distribution: 103.042
Normal distribution: 87.2083
Normal distribution: 112.446
Normal distribution: 102.555
Normal distribution: 90.1324
Normal distribution: 95.278
Normal distribution: 110.478
Normal distribution: 80.43
Normal distribution: 101.157
Normal distribution: 95.9593
Normal distribution: 119.843
Normal distribution: 96.1116
Normal distribution: 104.15
Normal distribution: 94.4328
Normal distribution: 97.9629
Normal distribution: 89.6683
Normal distribution: 105.784
Normal distribution: 92.8725
Normal distribution: 113.686
Normal distribution: 109.196
Normal distribution: 113.826
Normal distribution: 93.7604
Normal distribution: 98.8015
Normal distribution: 102.375
Normal distribution: 97.8095
Normal distribution: 101.471
Normal distribution: 102.083
Normal distribution: 94.9603
Normal distribution: 107.742
Normal distribution: 95.1097
Normal distribution: 109.723
Normal distribution: 115.253
Normal distribution: 101.296
Normal distribution: 91.9189
Normal distribution: 96.8736
Normal distribution: 106.408
Normal distribution: 97.7769
Normal distribution: 90.0967
Normal distribution: 94.8965
Normal distribution: 108.386
Normal distribution: 96.967
Normal distribution: 106.408
Normal distribution: 108.338
Normal distribution: 98.7292
Normal distribution: 115.377
Normal distribution: 85.8219
Normal distribution: 100.608
Normal distribution: 103.737
Normal distribution: 105.013
Normal distribution: 112.006
Normal distribution: 110.831
Normal distribution: 113.492
Normal distribution: 107.076
Normal distribution: 102.114
Normal distribution: 101.422
Normal distribution: 108.189
Normal distribution: 114.007
Normal distribution: 93.4933
Normal distribution: 84.9814
Normal distribution: 92.8097
Normal distribution: 90.3201
Normal distribution: 82.6701
Normal distribution: 94.6972
Normal distribution: 106.46
Normal distribution: 113.04
Normal distribution: 97.946
Normal distribution: 113.534
Normal distribution: 93.4873
Normal distribution: 108.997
Normal distribution: 84.8147
Normal distribution: 96.7856
Normal distribution: 97.8279
Normal distribution: 95.9185
Normal distribution: 107.07
Normal distribution: 92.1262
Normal distribution: 96.4959
Normal distribution: 123.187
Normal distribution: 87.1239
Normal distribution: 104.147
Normal distribution: 98.8908
Normal distribution: 92.7622
Normal distribution: 85.6457
Normal distribution: 110.31
Normal distribution: 102.916
Normal distribution: 107.542
Normal distribution: 109.558
Normal distribution: 104.314
Normal distribution: 94.3871
Normal distribution: 117.212
Normal distribution: 108.503
Normal distribution: 104.789
Normal distribution: 124.832
Normal distribution: 98.5929
Normal distribution: 106.027
Normal distribution: 94.0922
Normal distribution: 105.539
Normal distribution: 95.2899
Normal distribution: 93.4226
Normal distribution: 103.801
Normal distribution: 75.241
Normal distribution: 103.562
Normal distribution: 113.961
Normal distribution: 99.6632
Normal distribution: 100.357
Normal distribution: 100.877
Normal distribution: 91.7665
Normal distribution: 95.2144
Normal distribution: 105.463
Normal distribution: 80.602
Normal distribution: 98.1281
Normal distribution: 105.17
Normal distribution: 115.097
Normal distribution: 102.763
Normal distribution: 97.3895
Normal distribution: 100.6
Normal distribution: 118.31
Normal distribution: 103.62
Normal distribution: 81.8849
Normal distribution: 99.9072
Normal distribution: 112.993
Normal distribution: 124.143
Normal distribution: 110.105
Normal distribution: 90.6591
Normal distribution: 87.1402
Normal distribution: 96.6209
Normal distribution: 116.779
Normal distribution: 109.187
Normal distribution: 86.6592
Normal distribution: 122.383
Normal distribution: 95.9401
Normal distribution: 106.772
Normal distribution: 94.0501
Normal distribution: 101.015
Normal distribution: 106.316
Normal distribution: 108.371
Normal distribution: 89.6135
Normal distribution: 70.0876
Normal distribution: 89.1234
Normal distribution: 93.287
Normal distribution: 91.0091
Normal distribution: 101.543
Normal distribution: 81.4072
Normal distribution: 110.656
Normal distribution: 107.107
Normal distribution: 85.9463
Normal distribution: 100.043
Normal distribution: 111.882
Normal distribution: 98.3328
Normal distribution: 77.5091
Normal distribution: 101.762
Normal distribution: 123.789
Normal distribution: 98.6011
Normal distribution: 102.816
Normal distribution: 105.633
Normal distribution: 102.29
Normal distribution: 92.677
Normal distribution: 94.2936
Normal distribution: 104.737
Normal distribution: 91.1572
Normal distribution: 82.4843
Normal distribution: 104.683
Normal distribution: 108.194
Normal distribution: 87.5383
Normal distribution: 81.9873
Normal distribution: 100.666
Normal distribution: 107.306
Normal distribution: 98.798
Normal distribution: 103.056
Normal distribution: 103.984
Normal distribution: 95.11
Normal distribution: 95.3073
Normal distribution: 114.069
Normal distribution: 73.2781
Normal distribution: 103.663
Normal distribution: 111.445
Normal distribution: 95.4494
Normal distribution: 95.2536
Normal distribution: 108.198
Normal distribution: 97.6785
Normal distribution: 104.295
Normal distribution: 119.962
Normal distribution: 106.378
Normal distribution: 88.8008
Normal distribution: 96.6403
Normal distribution: 92.6461
Normal distribution: 89.984
Normal distribution: 95.8109
Normal distribution: 119.485
Normal distribution: 93.7419
Normal distribution: 96.8108
Normal distribution: 122.313
Normal distribution: 96.884
Normal distribution: 75.5849
Normal distribution: 92.5218
Normal distribution: 94.8646
Normal distribution: 100.349
Normal distribution: 99.5971
Normal distribution: 107.314
Normal distribution: 110.083
Normal distribution: 108.469
Normal distribution: 89.4347
Normal distribution: 125.845
Normal distribution: 106.116
Normal distribution: 107.495
Normal distribution: 98.7376
Normal distribution: 107.335
Normal distribution: 107.394
Normal distribution: 92.4994
Normal distribution: 92.3018
Normal distribution: 100.529
Normal distribution: 93.0215
Normal distribution: 97.7716
Normal distribution: 102.671
Normal distribution: 83.3672
Normal distribution: 109.355
Normal distribution: 102.707
Normal distribution: 80.6364
Normal distribution: 77.8604
Normal distribution: 110.49
Normal distribution: 104.789
Normal distribution: 94.4929
Normal distribution: 116.548
Normal distribution: 88.0161
Normal distribution: 113.414
Normal distribution: 98.0043
Normal distribution: 110.162
Normal distribution: 111.907
Normal distribution: 110.205
Normal distribution: 109.044
Normal distribution: 110.622
Normal distribution: 101.325
Normal distribution: 93.9263
Normal distribution: 97.5539
Normal distribution: 105.187
Normal distribution: 81.0886
Normal distribution: 85.7255
Normal distribution: 75.0634
Normal distribution: 108.416
Normal distribution: 86.4164
Normal distribution: 112.655
Normal distribution: 100.396
Normal distribution: 109.912
Normal distribution: 99.1524
Normal distribution: 114.73
Normal distribution: 87.7637
Normal distribution: 106.45
Normal distribution: 96.4508
Normal distribution: 105.474
Normal distribution: 110.409
Normal distribution: 111.881
Normal distribution: 101.962
Normal distribution: 93.5811
Normal distribution: 97.2924
Normal distribution: 123.587
Normal distribution: 113.021
Normal distribution: 99.3201
Normal distribution: 105.051
Normal distribution: 110.243
Normal distribution: 121.275
Normal distribution: 110.758
Normal distribution: 88.0783
Normal distribution: 105.166
Normal distribution: 105.592
Normal distribution: 117.726
Normal distribution: 84.5289
Normal distribution: 111.886
Normal distribution: 92.3169
Normal distribution: 91.4862
Normal distribution: 115.24
Normal distribution: 109.946
Normal distribution: 96.8114
Normal distribution: 91.7385
Normal distribution: 100.351
Normal distribution: 89.6291
Normal distribution: 81.9917
Normal distribution: 112.112
Normal distribution: 119.057
Normal distribution: 105.993
Normal distribution: 96.6556
Normal distribution: 97.1941
Normal distribution: 104.614
Normal distribution: 95.2851
Normal distribution: 117.149
Normal distribution: 110.593
Normal distribution: 103.67
Normal distribution: 118.859
Normal distribution: 109.138
Normal distribution: 103.214
Normal distribution: 90.9816
Normal distribution: 101.259
Normal distribution: 100.986
Normal distribution: 106.372
Normal distribution: 114.572
Normal distribution: 92.252
Normal distribution: 97.3103
Normal distribution: 94.916
Normal distribution: 89.4608
Normal distribution: 89.6554
Normal distribution: 95.6812
Normal distribution: 82.2103
Normal distribution: 118.027
Normal distribution: 120.995
Normal distribution: 97.0905
Normal distribution: 111.567
Normal distribution: 97.8433
Normal distribution: 102.173
Normal distribution: 109.282
Normal distribution: 97.9103
Normal distribution: 97.9572
Normal distribution: 111.015
Normal distribution: 88.7456
Normal distribution: 107.091
Normal distribution: 97.0672
Normal distribution: 83.6199
Normal distribution: 91.9612
Normal distribution: 110.852
Normal distribution: 106.095
Normal distribution: 113.892
Normal distribution: 79.9899
Normal distribution: 106.454
Normal distribution: 85.478
Normal distribution: 108.237
Normal distribution: 113.902
Normal distribution: 106.263
Normal distribution: 106.315
Normal distribution: 95.55
Normal distribution: 102.305
Normal distribution: 107.383
Normal distribution: 106.683
Normal distribution: 111.055
Normal distribution: 92.0168
Normal distribution: 116.04
Normal distribution: 103.272
Normal distribution: 91.8005
Normal distribution: 98.4199
Normal distribution: 113.829
Normal distribution: 102.645
Normal distribution: 103.995
Normal distribution: 98.2559
Normal distribution: 107.657
Normal distribution: 90.1141
Normal distribution: 98.4381
Normal distribution: 112.026
Normal distribution: 110.152
Normal distribution: 97.7776
Normal distribution: 95.4085
Normal distribution: 107.17
Normal distribution: 86.4677
Normal distribution: 102.438
Normal distribution: 109.713
Normal distribution: 112.797
Normal distribution: 95.3735
Normal distribution: 95.8992
Normal distribution: 99.0949
Normal distribution: 89.1142
Normal distribution: 102.992
Normal distribution: 91.8559
Normal distribution: 76.5078
Normal distribution: 83.8471
Normal distribution: 102.756
Normal distribution: 91.4684
Normal distribution: 74.0996
Normal distribution: 108.613
Normal distribution: 88.3028
Normal distribution: 92.5938
Normal distribution: 87.4987
Normal distribution: 102.683
Normal distribution: 106.086
Normal distribution: 100.417
Normal distribution: 110.558
Normal distribution: 92.8425
Normal distribution: 86.3323
Normal distribution: 103.679
Normal distribution: 97.043
Normal distribution: 102.228
Normal distribution: 91.6709
Normal distribution: 118.766
Normal distribution: 99.4774
Normal distribution: 88.7861
Normal distribution: 88.5542
Normal distribution: 102.413
Normal distribution: 115.277
Normal distribution: 83.415
Normal distribution: 115.674
Normal distribution: 106.865
Normal distribution: 89.5816
Normal distribution: 88.1411
Normal distribution: 94.5734
Normal distribution: 105.599
Normal distribution: 108.039
Normal distribution: 102.83
Normal distribution: 96.7236
Normal distribution: 105.69
Normal distribution: 89.4724
Normal distribution: 97.3707
Normal distribution: 107.012
Normal distribution: 110.567
Normal distribution: 97.8939
Normal distribution: 111.727
Normal distribution: 93.3343
Normal distribution: 104.175
Normal distribution: 95.8728
Normal distribution: 102.321
Normal distribution: 94.4842
Normal distribution: 94.9925
Normal distribution: 90.4774
Normal distribution: 92.1574
Normal distribution: 94.5605
Normal distribution: 97.2663
Normal distribution: 108.851
Normal distribution: 119.495
Normal distribution: 114.98
Normal distribution: 111.43
Normal distribution: 101.939
Normal distribution: 93.1704
Normal distribution: 98.2265
Normal distribution: 100.449
Normal distribution: 112.628
Normal distribution: 95.7353
Normal distribution: 114.905
Normal distribution: 95.1558
Normal distribution: 109.188
Normal distribution: 110.519
Normal distribution: 118.973
Normal distribution: 113.102
Normal distribution: 99.6547
Normal distribution: 110.147
Normal distribution: 96.9239
Normal distribution: 102.198
Normal distribution: 100.679
Normal distribution: 93.8296
Normal distribution: 110.708
Normal distribution: 88.3733
Normal distribution: 109.577
Normal distribution: 117.502
Normal distribution: 102.436
Normal distribution: 113.355
Normal distribution: 108.818
Normal distribution: 104.961
Normal distribution: 106.967
Normal distribution: 102.64
Normal distribution: 109.749
Normal distribution: 99.4982
Normal distribution: 106.761
Normal distribution: 110.871
Normal distribution: 101.202
Normal distribution: 109.992
Normal distribution: 98.089
Normal distribution: 102.606
Normal distribution: 105.188
Normal distribution: 120.762
Normal distribution: 95.8697
Normal distribution: 112.599
Normal distribution: 96.0212
Normal distribution: 115.304
Normal distribution: 101.173
Normal distribution: 89.0297
Normal distribution: 110.516
Normal distribution: 91.5459
Normal distribution: 103.873
Normal distribution: 94.1549
Normal distribution: 113.544
Normal distribution: 99.218
Normal distribution: 83.1582
Normal distribution: 96.9599
Normal distribution: 103.417
Normal distribution: 93.4617
Normal distribution: 107.226
Normal distribution: 98.0813
Normal distribution: 101.304
Normal distribution: 101.582
Normal distribution: 102.51
Normal distribution: 109.761
Normal distribution: 90.6201
Normal distribution: 95.7512
Normal distribution: 102.628
Normal distribution: 98.544
Normal distribution: 106.791
Normal distribution: 104.204
Normal distribution: 97.9898
Normal distribution: 98.1549
Normal distribution: 103.853
Normal distribution: 98.5596
Normal distribution: 99.0998
Normal distribution: 87.5786
Normal distribution: 100.317
Normal distribution: 92.0863
Normal distribution: 77.9755
Normal distribution: 83.5359
Normal distribution: 106.646
Normal distribution: 96.4459
Normal distribution: 102.914
Normal distribution: 105.049
Normal distribution: 90.9792
Normal distribution: 105.083
Normal distribution: 90.8475
Normal distribution: 101.961
Normal distribution: 113.879
Normal distribution: 96.5841
Normal distribution: 110.529
Normal distribution: 102.463
Normal distribution: 77.4375
Normal distribution: 75.3665
Normal distribution: 110.797
Normal distribution: 97.6916
Normal distribution: 100.266
Normal distribution: 107.272
Normal distribution: 92.8774
Normal distribution: 89.4683
Normal distribution: 103.091
Normal distribution: 89.7332
Normal distribution: 98.0392
Normal distribution: 101.209
Normal distribution: 97.7692
Normal distribution: 96.3921
Normal distribution: 94.6458
Normal distribution: 96.6958
Normal distribution: 110.148
Normal distribution: 103.965
Normal distribution: 105.992
Normal distribution: 105.689
Normal distribution: 88.4271
Normal distribution: 94.4285
Normal distribution: 100.936
Normal distribution: 87.9805
Normal distribution: 79.2061
Normal distribution: 116.332
Normal distribution: 107.656
Normal distribution: 68.3557
Normal distribution: 115.089
Normal distribution: 105.192
Normal distribution: 94.5374
Normal distribution: 110.981
Normal distribution: 104.344
Normal distribution: 96.2027
Normal distribution: 99.6902
Normal distribution: 113.738
Normal distribution: 89.5946
Normal distribution: 74.4693
Normal distribution: 108.167
Normal distribution: 106.061
Normal distribution: 107.134
Normal distribution: 84.6884
Normal distribution: 108.301
Normal distribution: 106.057
Normal distribution: 87.2862
Normal distribution: 88.1856
Normal distribution: 107.811
Normal distribution: 104.807
Normal distribution: 92.4527
Normal distribution: 116.593
Normal distribution: 118.912
Normal distribution: 108.287
Normal distribution: 95.8282
Normal distribution: 90.7851
Normal distribution: 88.8206
Normal distribution: 101.611
Normal distribution: 90.8661
Normal distribution: 108.928
Normal distribution: 79.6627
Normal distribution: 86.127
Normal distribution: 106.821
Normal distribution: 112.101
Normal distribution: 109.384
Normal distribution: 89.5508
Normal distribution: 114.622
Normal distribution: 96.3587
Normal distribution: 89.0338
Normal distribution: 75.2377
Normal distribution: 122.074
Normal distribution: 114.796
Normal distribution: 103.749
Normal distribution: 114.373
Normal distribution: 80.673
Normal distribution: 87.3858
Normal distribution: 89.9167
Normal distribution: 104.075
Normal distribution: 95.3313
Normal distribution: 103.634
Normal distribution: 130.627
Normal distribution: 102.592
Normal distribution: 104.891
Normal distribution: 92.9296
Normal distribution: 104.107
Normal distribution: 110.39
Normal distribution: 104.55
Normal distribution: 101.126
Normal distribution: 102.941
Normal distribution: 84.0924
Normal distribution: 113.801
Normal distribution: 115.975
Normal distribution: 124.614
Normal distribution: 93.8368
Normal distribution: 102.018
Normal distribution: 107.709
Normal distribution: 103.651
Normal distribution: 93.4696
Normal distribution: 106.955
Normal distribution: 94.9266
Normal distribution: 85.895
Normal distribution: 99.6891
Normal distribution: 106.142
Normal distribution: 110.966
Normal distribution: 81.2946
Normal distribution: 74.6366
Normal distribution: 103.323
Normal distribution: 103.95
Normal distribution: 101.666
Normal distribution: 99.4039
Normal distribution: 100.22
Normal distribution: 102.956
Normal distribution: 108.167
Normal distribution: 95.8655
Normal distribution: 95.1625
Normal distribution: 100.47
Normal distribution: 113.929
Normal distribution: 104.625
Normal distribution: 122.212
Normal distribution: 88.6193
Normal distribution: 105.23
Normal distribution: 95.5291
Normal distribution: 93.4262
Normal distribution: 118.11
Normal distribution: 102.385
Normal distribution: 116.972
Normal distribution: 94.8478
Normal distribution: 104.025
Normal distribution: 98.3952
Normal distribution: 100.304
Normal distribution: 101.204
Normal distribution: 106.227
Normal distribution: 117.881
Normal distribution: 96.3082
Normal distribution: 103.7
Normal distribution: 106.466
Normal distribution: 106.779
Normal distribution: 91.6121
Normal distribution: 92.518
Normal distribution: 101.693
Normal distribution: 109.82
Normal distribution: 107.444
Normal distribution: 99.7316
Normal distribution: 116.365
Normal distribution: 88.1991
Normal distribution: 93.6419
Normal distribution: 112.763
Normal distribution: 126.819
Normal distribution: 100.164
Normal distribution: 118.552
Normal distribution: 119.381
Normal distribution: 74.8847
Normal distribution: 85.4073
Normal distribution: 94.2105
Normal distribution: 91.6279
Normal distribution: 117.118
Normal distribution: 99.2106
"""

# Function to extract normal distribution values from text
def extract_normal_data(text):
    lines = text.strip().split('\n')
    data = [float(line.split(":")[1].strip()) for line in lines if "Normal distribution" in line]
    return np.array(data)

# Extract the normal distribution data from text
data = extract_normal_data(text_data)

# Calculate the mean and standard deviation
mean = np.mean(data)
std_dev = np.std(data)

# Generate a range of values based on the extracted data
x_values = np.linspace(mean - 3*std_dev, mean + 3*std_dev, 100)

# Calculate the normal distribution curve
y_values = (1 / (std_dev * np.sqrt(2 * np.pi))) * np.exp(-0.5 * ((x_values - mean) / std_dev) ** 2)

# Plot the normal distribution curve
plt.figure(figsize=(10, 6))
plt.plot(x_values, y_values, label=f'Normal Distribution\nMean: {mean:.2f}, Std Dev: {std_dev:.2f}', color='blue')
plt.hist(data, bins=10, density=True, alpha=0.6, color='gray', edgecolor='black', label='Data Histogram')

plt.title('Normal Distribution from Extracted Data')
plt.xlabel('Value')
plt.ylabel('Density')
plt.grid(True)
plt.legend()
plt.show()

