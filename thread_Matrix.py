import time
import random
import threading


def matrix(A, B):
    results = []
    try:
        if len(A[0]) != len(B):
            raise
        for i in range(len(A)):
            rows = []
            for j in range(len(B[0])):
                item = 0
                for k in range(len(A[0])):
                    item += A[i][k] * B[k][j]
                rows.append(item)
            results.append(rows)
        return results
    except:
        return None


def matrixThread_2(A, B, idx, i, j):
    global item
    item += A[i][idx] * B[idx][j]
    # return A[i][idx] * B[idx][j]


def matrixThread_1(A, B, i, j):
    global item
    item = 0
    threads = []
    for idx in range(len(A[0])):
        threads.append(
            threading.Thread(target=matrixThread_2, args=(A, B, idx, i, j)))
        threads[idx].start()
        # item += matrixThread_2(A, B, idx, i, j)
    for idx in range(len(A[0])):
        threads[idx].join()
    return item


def matrixThread(A, B, result):
    try:
        if len(A[0]) != len(B):
            raise
        for i in range(len(A)):
            for j in range(len(B[0])):
                if result[i][j] == 0:
                    result[i][j] = matrixThread_1(A, B, i, j)
        return result
    except:
        return None


def matrixInit(A, B):
    result = []
    for i in range(len(A)):
        rows = []
        for j in range(len(B[0])):
            item = 0
            rows.append(item)
        result.append(rows)
    return result


def randomMatrix():
    upLimit, downLimit, num = 50, 25, 10
    Ai, k, Bj = random.randint(downLimit, upLimit), upLimit, random.randint(
        downLimit, upLimit)
    A, B = [], []
    for i in range(Ai):
        rows = []
        for j in range(k):
            itemNow = random.randint(-num, num)
            rows.append(itemNow)
        A.append(rows)
    for i in range(k):
        rows = []
        for j in range(Bj):
            itemNow = random.randint(-num, num)
            rows.append(itemNow)
        B.append(rows)
    return A, B


def main():
    A, B = randomMatrix()

    start_time = time.time_ns()
    resultMatrix_1 = matrix(A, B)
    end_time = time.time_ns()

    # print(resultMatrix_1)
    print(end_time - start_time)

    result = matrixInit(A, B)

    start_time1 = time.time_ns()
    resultMatrix_2 = matrixThread(A, B, result)  # (B, A), (A, B)
    end_time1 = time.time_ns()

    # print(resultMatrix_2)
    print(end_time1 - start_time1)


if __name__ == '__main__':
    main()
    exit()
