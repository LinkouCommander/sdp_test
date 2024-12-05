import matplotlib.pyplot as plt

# 假設這是你收集到的數據
threads = [1, 2, 4, 8, 16, 32, 64, 128, 256]  # 執行緒數量
execution_time_serial = [
    3.07148,  # NT=1
    3.06998,  # NT=2
    3.0698,   # NT=4
    3.07761,  # NT=8
    3.07123,  # NT=16
    3.0697,   # NT=32
    3.06901,  # NT=64
    3.07532,  # NT=128
    3.07211   # NT=256
]

execution_time_mm_parallel = [
    2.86862,  # NT=1
    1.47077,  # NT=2
    0.797717, # NT=4
    0.439615, # NT=8
    0.397492, # NT=16
    0.311671, # NT=32
    0.295317, # NT=64
    0.29008,  # NT=128
    0.296632  # NT=256
]

execution_time_bmm = [
    7.63258,  # NT=1
    3.86254,  # NT=2
    2.03583,  # NT=4
    1.08675,  # NT=8
    1.05852,  # NT=16
    0.83262,  # NT=32
    0.758617, # NT=64
    0.750388, # NT=128
    0.762257  # NT=256
]

execution_time_online_softmax = [
    2.86446,  # NT=1
    1.46936,  # NT=2
    0.794858, # NT=4
    0.441542, # NT=8
    0.399005, # NT=16
    0.336044, # NT=32
    0.292457, # NT=64
    0.28745,  # NT=128
    0.298897  # NT=256
]

execution_time_fused = [
    2.77346,  # NT=1
    1.39508,  # NT=2
    0.731724, # NT=4
    0.378574, # NT=8
    0.327295, # NT=16
    0.231928, # NT=32
    0.253311, # NT=64
    0.219412, # NT=128
    0.230289  # NT=256
]

# 創建折線圖
plt.figure(figsize=(12, 8))
# plt.plot(num_threads, execution_time1, marker='o', linestyle='-', color='b')
# plt.plot(num_threads, execution_time2, marker='o', linestyle='-', color='r')
# plt.plot(num_threads, execution_time3, marker='o', linestyle='-', color='c')

plt.plot(threads, execution_time_serial, marker='o', linestyle='-', label='sdp_serial')
plt.plot(threads, execution_time_mm_parallel, marker='o', linestyle='-', label='sdp_mm_parallel')
plt.plot(threads, execution_time_bmm, marker='o', linestyle='-', label='sdp_bmm')
plt.plot(threads, execution_time_online_softmax, marker='o', linestyle='-', label='sdp_online_softmax')
plt.plot(threads, execution_time_fused, marker='o', linestyle='-', label='sdp_fused')

# 設置標題和標籤
plt.title('Matrix Multiplication (Number of Threads vs Execution Time)')
plt.xlabel('Number of Threads (p x p)')
plt.ylabel('Execution Time (seconds)')
plt.legend()

# 顯示圖表
plt.grid(True)
plt.show()
