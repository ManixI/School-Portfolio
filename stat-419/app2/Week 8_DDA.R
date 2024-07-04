###Descriptive Discriminant Analysis

#install.packages("DiscriMiner")
#no longer available!

#MASS package can do lda() linear discriminant analysis which is where both DDA and PDA can be assessed
install.packages("MASS")
library(MASS)

#import data Manager Performance Data and name data
#check if experience can be used as grouping variable
table(data$jobs)
for (i in 1:nrow(data)){
  if (data$jobs[i]>1){
    data$jobs_rec[i]=2
  } else {
    data$jobs_rec[i]=data$jobs[i]
  }
}
table(data$jobs_rec)

mvdata=data[,c("selfconf","extrav","perform","jobs_rec")]

##check assumptions
#check normality
library(stats)
D2=mahalanobis(mvdata[,1:3],colMeans(mvdata[,1:3]),cov(mvdata[,1:3]))
plot(density(D2,bw=.5),
     main="Squared Mahalanobis distances");rug(D2)
qqplot(qchisq(ppoints(100),df=2),D2,
       main=expression("Q-Q plot of Mahalanobis"*~D^2*
                         "vs. quantiles of"*~chi[2]^2))
abline(0,1,col="red")
#one value appears far off

library(rstatix)
mshapiro_test(mvdata[,1:3])
#significant non-normality?
#lets say cutoff was .05

#Identify extreme Maholonobi's distance
which.max(D2)
#remove from data for analysis
mvdata2=mvdata[-33,]

#check normality for new dataset
library(stats)
D2b=mahalanobis(mvdata2[,1:3],colMeans(mvdata2[,1:3]),cov(mvdata2[,1:3]))
plot(density(D2b,bw=.5),
     main="Squared Mahalanobis distances");rug(D2b)
qqplot(qchisq(ppoints(99),df=2),D2b,
       main=expression("Q-Q plot of Mahalanobis"*~D^2*
                         "vs. quantiles of"*~chi[2]^2))
abline(0,1,col="red")

library(rstatix)
mshapiro_test(mvdata2[,1:3])
#now can assume mvnorm

#check equality of variance covariance matrices
library(rstatix)
box_m(mvdata2[,1:3],mvdata2$jobs_rec)
#equality can be assumed

##analysis
#standardize variables
zmvdata2=as.data.frame(cbind(scale(mvdata2$selfconf),scale(mvdata2$extrav),scale(mvdata2$perform),
                             mvdata2$jobs_rec))
colnames(zmvdata2)=c("selfconf","extrav","perform","jobs_rec")
#check if it worked
summary(zmvdata2)
sd(zmvdata2$selfconf)

library(MASS)
dda1=lda(zmvdata2[,1:3],grouping=zmvdata2$jobs_rec)
dda1
plot(dda1)
plot(dda1,dimen=1,type="both")

###try with other package
library(DiscriMiner)
dda2=desDA(zmvdata2[,1:3], zmvdata2$jobs_rec,covar="total")
dda2
manova2=manova(cbind(zmvdata2$selfconf,zmvdata2$extrav,zmvdata2$perform)~zmvdata2$jobs_rec)
overallWilks=summary(manova2,test="Wilks")

#standardized coefficients
zcoef=round(dda2$discrivar[-1,],3)
zcoef
#structure coefficients
str_coef=round(dda2$discor,3)
str_coef

#need to get group centroids
#get means per group on all outcomes
library(DiscriMiner)
groupmeans=groupMeans(zmvdata2[,1:3], zmvdata2$jobs_rec)
groupmeans
centDF1_0=zcoef[1,1]*groupmeans[1,1]+zcoef[2,1]*groupmeans[2,1]+zcoef[3,1]*groupmeans[3,1]
centDF1_1=zcoef[1,1]*groupmeans[1,2]+zcoef[2,1]*groupmeans[2,2]+zcoef[3,1]*groupmeans[3,2]
centDF1_2=zcoef[1,1]*groupmeans[1,3]+zcoef[2,1]*groupmeans[2,3]+zcoef[3,1]*groupmeans[3,3]
centDF1=c(centDF1_0,centDF1_1,centDF1_2)

centDF2_0=zcoef[1,2]*groupmeans[1,1]+zcoef[2,2]*groupmeans[2,1]+zcoef[3,2]*groupmeans[3,1]
centDF2_1=zcoef[1,2]*groupmeans[1,2]+zcoef[2,2]*groupmeans[2,2]+zcoef[3,2]*groupmeans[3,2]
centDF2_2=zcoef[1,2]*groupmeans[1,3]+zcoef[2,2]*groupmeans[2,3]+zcoef[3,2]*groupmeans[3,3]
centDF2=c(centDF2_0,centDF2_1,centDF2_2)
dda2_centroids=cbind(centDF1,centDF2)
rownames(dda2_centroids)=c("selfconf","extrav","perform")

##final output info
#overall fit
overallWilks
#significance
dda2$values
#eigenvalues and proportion of total for each DDA
1-.94942
#etasquared for both functions together
#nonsignificant DDA... we will ignore that for now!
#BOTH DDAs only explain just over 5% of group differences

#which DVs
zcoef
str_coef

#Which groups
dda2_centroids

