#This is a note

#can adjust global options

#set where r will look for and save files
setwd("~/dev/stat-419/week-1")
getwd()
#file paths are a COMMON error source!!

#setwd("C:/Users/CISER-Admin/OneDrive - Washington State University (email.wsu.edu)/WSU/Teaching/R")

#to set a value
1
1+1
value = 1
value

value+value

#create vector
vector = c(1, 2, 3)
vector

vector2 = c(5,4,2)

#to remove an object from environment
rm(vector2)
#remove(vector2)

vector2 = c(5,4,2)

#create matrix
matrix = cbind(vector,vector2)
matrix
is.matrix(matrix)

words=c("Trey","is","cool")
#failure!
#words2=c(Trey, is, cool)

matrix2=cbind(matrix,words)
is.matrix(matrix2)

#change to data frame
dataframe=as.data.frame(matrix)
is.data.frame(dataframe)

dataframe2=cbind(dataframe, words)
rm(dataframe2)
cbind(dataframe, words)

matrix3=rbind(vector, vector2)
dataframe3=as.data.frame(matrix3)
dataframe3=cbind(dataframe3, words)

emptyrow=c(NA,NA,NA)
dataframe3=rbind(dataframe3,emptyrow)
dataframe3=cbind(dataframe3,words)

#takes up more space but more flexible

#r can do math
2+2

#it can also save math
####Housing
###Monthly
HousingCost=1700+75+80+50

AnnualHouse=HousingCost*12

#I can ask for specific column/row in set
matrix2[,3]
matrix2[1,]

matrix$vector2 #doesn't work
dataframe=as.data.frame(matrix2)
dataframe$vector2
is.numeric(dataframe$vector2)
as.numeric(dataframe$vector2)
NUMvector2=as.numeric(dataframe$vector2)
dataframe4=cbind(dataframe,NUMvector2)
dataframe4=dataframe4[,-2]

dataframe$vector2
mean(as.numeric(dataframe$vector2))

dataframe4$NUMvector2-mean(dataframe4$NUMvector2)

#import data
#can point and click or use code
BasicsData <- read.csv("BasicsData.csv")
#this is to import csv
#other data types require different commands
#try importing for SPSS data version

#can do stats on data
data=BasicsData

mean(data$Quality)
mean(data$NoEngineers)
median(data$NoEngineers)
median(data$NoEngineers, na.rm=TRUE)

sd(data$Quality)
sd(data$NoEngineers)


range(data$Quality)
summary(data$Quality)
summary(data$NoEngineers)

hist(data$Quality, main="Histogram", xlab="Party Quality")
hist(data$NoEngineers)
?hist()

plot(data$Quality, data$NoEngineers)
?plot()

plot(data$NoEngineers,data$Quality)

boxplot(data$NoEngineers)
