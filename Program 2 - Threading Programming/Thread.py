import os, sys, time, getopt
import requests, threading
from bs4 import BeautifulSoup
from PIL import Image, ImageFile
from urllib.request import urlretrieve
"""
@author: Ping
"""

ImageFile.LOAD_TRUNCATED_IMAGES = True


def getURLFromWebsite(urlTmp, urlBook):
    response = requests.get(urlTmp)
    response.encoding = 'big5-hkscs'
    soup = BeautifulSoup(response.text, "html.parser")
    # take out url and name by tag
    results = soup.find_all("a")
    urls = []
    BookName = ""
    for result in results:
        if type(result.get("href")) == str:
            if 'comic' in result.get("href"):
                urls.append(result)
                if '清除記錄' in result.text:
                    urls.clear()
                elif urlBook in result.get("href") and '#' not in result.get(
                        "href"):
                    BookName = result.text
    # remove bad items
    for i in range(2):
        urls.pop(0)
    checkLen = len(urls) - int(urls[0].text) - 2
    for i in range(checkLen):
        urls.pop(-1)
    urls.pop(0)
    # take out page number by tag
    results = soup.find_all("font")
    urlsNum = []
    for result in results:
        if '(' in result.text:
            urlsNum.append(result.text[1:-2])
    # check content length
    if len(urls) != len(urlsNum):
        print("Bad Trying for url :", urlTmp)
        return
    # build the list which content url, book number, page number
    finalList = []
    for i in range(len(urls)):
        tmp = []
        herf = urls[i].get("href")
        tmp.append(herf[herf.find('/comic/') + len('/comic/'):herf.find('.')])
        tmp.append(int(urls[i].text[2:-2]))
        tmp.append(urlsNum[i])
        finalList.append(tmp)
    return finalList, BookName


def getURLFromURL(preUrl, nowUrl):
    nowUrl = nowUrl + '.html'
    # this is very important
    headers = {'referer': preUrl}
    response = requests.get(nowUrl, headers=headers)
    response.encoding = 'big5-hkscs'
    soup = BeautifulSoup(response.text, "html.parser")
    # take out url by tag
    results = soup.find_all("img")
    urlTarget = ''
    for result in results:
        if 'asp' in result.get("src"):
            urlTarget = result.get("src")
            break
    # get the baseID by urlTarget
    response_ = requests.get(urlTarget, headers=headers)
    downloadBase = response_.url[response_.url.find('com/') + len('com/'):][:5]
    return downloadBase


def getListFromUrl(url, bookNumber, bookSubNumber, pageNumber):
    ansList = []
    for i in range(pageNumber):
        pageStr = str(i + 1).zfill(3)
        newUrl = url + '/' + bookNumber + '/' + bookSubNumber + '/' + pageStr + '.jpg'
        ansList.append([newUrl, bookNumber, bookSubNumber, pageStr])
    return ansList


def getUrlLoop(urlList, baseUrl, folderUrl):
    totalList = []
    baseID = getURLFromURL(baseUrl, baseUrl + folderUrl + urlList[0][0])
    for i in range(len(urlList)):
        print("\rBook complete :", i + 1, "/", len(urlList), end="")
        ansList = getListFromUrl(baseUrl + baseID, urlList[i][0][:4],
                                 str(urlList[i][1]).zfill(3),
                                 int(urlList[i][2]))
        if ansList != None:
            totalList.append(ansList)
    return totalList


def pathCreate(pathName, bookNum, booksubNum):
    checker = True
    try:
        os.mkdir(pathName)
    except:
        print("\rWarning :", pathName, "is already exist.", end="")
        checker = False
    pathName += "/" + bookNum
    try:
        os.mkdir(pathName)
    except:
        print("\rWarning :", pathName, "is already exist.", end="")
        checker = False
    for booksubNum_ in booksubNum:
        try:
            os.mkdir(pathName + "/" + booksubNum_)
        except:
            print("\rWarning :",
                  pathName + "/" + booksubNum_,
                  "is already exist.",
                  end="")
            checker = False
    if not checker:
        print()
    return True


def createFileDirectory(downloadList, fileBase):
    bookNum, booksubNum = 0, []
    for bookList in downloadList:
        for pageInfo in bookList:
            bookNum = pageInfo[1]
            if pageInfo[2] not in booksubNum:
                booksubNum.append(pageInfo[2])
    pathCreate(fileBase, bookNum, booksubNum)
    return fileBase + "/" + bookNum


def urlIsDownloadable(inputUrl):
    responseHead = requests.head(inputUrl, allow_redirects=True)
    contentType = responseHead.headers.get('content-type')
    if 'text' in contentType.lower():
        return False
    elif 'html' in contentType.lower():
        return False
    return True


def downloadPic(downloadInfo, fileBase, preUrl):
    for i in range(3):
        fileBase = fileBase + "/" + downloadInfo[i + 1]
    fileName = os.path.join(fileBase + ".png")
    # download the pic with chunk set
    headers = {
        'User-Agent':
        'Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.8.1.6) Gecko/20070802 SeaMonkey/1.1.4',
        'referer': preUrl
    }
    response = requests.get(downloadInfo[0], headers=headers)
    with open(fileName, 'wb') as fileNow:
        fileNow.write(response.content)
    return True


def downloadByLoop(downloadList, fileBase, preUrl, maxThreadNum):
    # download by thread
    totalCount = 0
    threads = []
    for bookList in downloadList:
        for i, pageInfo in enumerate(bookList):
            print("\rBook Num, Book subNum, Page Num :",
                  pageInfo[1],
                  pageInfo[2],
                  pageInfo[3],
                  end="")
            totalCount += 1
            threadNow = threading.Thread(target=downloadPic,
                                         args=(pageInfo, fileBase, preUrl))
            threads.append(threadNow)
    print()
    threadStartList = []
    for i, threadNow in enumerate(threads):
        print("\rPage complete :", i + 1, "/", totalCount, end="")
        threadNow.start()
        threadStartList.append(threadNow)
        if i % maxThreadNum == 0:
            for threadJoinNow in threadStartList:
                threadJoinNow.join(timeout=2.0)
            threadStartList = []
    return


def picToPDF(PicPathBase, BookName, pageNumber):
    check = False
    while not check:
        for dirPath, dirNames, fileNames in os.walk(PicPathBase + "/" +
                                                    BookName):
            imageList, fName, fLen = [], '', 0
            for i, f in enumerate(fileNames):
                # print(os.path.join(dirPath, f))
                if i == 0:
                    fName = f
                if '.jpg' in f or '.png' in f:
                    inputImage = Image.open(os.path.join(dirPath, f))
                    inputImage = inputImage.convert('RGB')
                    imageList.append(inputImage)
                    fLen += 1
            if len(imageList) != 0:
                imageList.pop(0)
                if '.jpg' in fName or '.png' in fName:
                    inputImageNew = Image.open(os.path.join(dirPath, fName))
                    inputImageNew = inputImageNew.convert('RGB')
                    inputImageNew.save(PicPathBase + '/' + BookName + '.pdf',
                                       save_all=True,
                                       append_images=imageList)
                    if len(imageList) + 1 != fLen or fLen != pageNumber:
                        check = False
                    else:
                        check = True
    return


def mergeByLoop(downloadList, bookBase, maxThreadNum):
    threads = []
    for i, bookList in enumerate(downloadList):
        print("\rBook loading :", i + 1, "/", len(downloadList), end="")
        bookSubName, pageNumber = bookList[0][2], int(bookList[-1][3])
        threadNow = threading.Thread(target=picToPDF,
                                     args=(bookBase, bookSubName, pageNumber))
        threads.append(threadNow)
    print()
    threadStartList = []
    for i, threadNow in enumerate(threads):
        print("\rBook complete :", i + 1, "/", len(threads), end="")
        threadNow.start()
        threadStartList.append(threadNow)
        if i % maxThreadNum == 0:
            for threadJoinNow in threadStartList:
                threadJoinNow.join(timeout=2.0)
            threadStartList = []
    return


def startWebCrawler(bookNum, maxThreadNum, PDFmerge):
    # pre stage
    dirBase = "D:/Programming/picture"
    urlOrg = "https://www.cartoonmad.com/"
    urlFolder = "comic/"
    urlBook = bookNum + '.html'  # 1221, 8459

    # stage1
    print("\nStart the web crawler")
    stage1 = time.time()
    urlList, BookName = getURLFromWebsite(urlOrg + urlFolder + urlBook,
                                          urlFolder + urlBook)
    stage1_ = time.time()
    print("End stage1. It took about", "%.3f" % (stage1_ - stage1), "s.")

    if BookName == '':
        print("Can't find this book.")
        return
    check = input("\nBook Name is " + BookName + " continue? (Y/N) ")
    if check != 'Y':
        return

    # stage2
    print("\nStart get all pic url")
    stage2 = time.time()
    totalList = getUrlLoop(urlList, urlOrg, urlFolder)
    stage2_ = time.time()
    print("\nEnd stage2. It took about", "%.3f" % (stage2_ - stage2), "s.")

    # stage3
    print("\nCreate the file directory")
    stage3 = time.time()
    bookBase = createFileDirectory(totalList, dirBase)
    stage3_ = time.time()
    print("End stage3. It took about", "%.3f" % (stage3_ - stage3), "s.")

    # stage4
    print("\nStart download all pic")
    stage4 = time.time()
    downloadByLoop(totalList, dirBase, urlOrg, maxThreadNum)
    stage4_ = time.time()
    print("\nEnd stage4. It took about", "%.3f" % (stage4_ - stage4), "s.")

    if PDFmerge:
        # stage5
        print("\nStart merge all download")
        stage5 = time.time()
        mergeByLoop(totalList, bookBase, maxThreadNum)
        stage5_ = time.time()
        print("\nEnd stage5. It took about", "%.3f" % (stage5_ - stage5), "s.")

        # End stage
        print("\nEnd of all. Total time spends", "%.3f" % (stage5_ - stage1),
              "s.")
    else:
        # End stage
        print("\nEnd of all. Total time spends", "%.3f" % (stage4_ - stage1),
              "s.")
    return


def main(argv, bookNumInput):
    outputStr = 'Thread.py [-h] -b <bookName> -t <threadNum> [-p]'
    helpStr = 'Thread.py\n-h : for this message.\n-b <bookName> : for input book name on the url.\n-t <threadNum> : for the max thread number.\n-p : (optional) for enable merging and converting to PDF.'
    try:
        opts, args = getopt.getopt(argv, "hb:pt:", ["bookNum=", "threadNum="])
    except getopt.GetoptError:
        print(outputStr)
        sys.exit(2)
    checkPDF = False
    checkNum = 0
    for opt, arg in opts:
        if opt == '-h':
            print(helpStr)
            sys.exit(1)
        elif opt in ("-b", "--bookNum"):
            bookNum = str(arg)
            checkNum += 1
        elif opt in ("-t", "--threadNum"):
            try:
                threadNum = int(arg)
                checkNum += 1
            except:
                print("Please input max thread num, ex. 24")
                sys.exit(1)
        elif opt in ("-p"):
            checkPDF = True
    if bookNumInput != "":
        bookNum = bookNumInput
    if checkNum == 2:
        startWebCrawler(bookNum, threadNum, checkPDF)
    else:
        print(outputStr)
    return


def clearScreen():
    os.system('cls')
    return


if __name__ == '__main__':
    # clearScreen()
    checker = True
    bookNum = ""
    while checker:
        main(sys.argv[1:], bookNum)
        continueCheck = input("\nContinue the web crawler? (Y/N) ")
        if continueCheck == 'Y':
            checker = True
            bookNum = str(input("Please input next book num. "))
        else:
            checker = False
    exit()