####Multi-way MANOVA with DDA Post-hoc

###install.packages("DiscriMiner") from saved files
###import Barbados2019 SPSS data file from Canvas
mvdata=cbind(scale(data[,173:177]),data[,8],data[,23])
mvdata$Sex=as.factor(mvdata$Sex)
mvdata$OneorTwoparents=as.factor(mvdata$OneorTwoparents)
mvdata=na.omit(mvdata)

###check assumptions
##check normality
library(stats)
D2=mahalanobis(mvdata[,1:5],colMeans(mvdata[,1:5]),cov(mvdata[,1:5]))
plot(density(D2,bw=.5),
     main="Squared Mahalanobis distances");rug(D2)
qqnorm(D2,col="blue",cex=.5,pch=19)
qqline(D2,col="red",lwd=1)

library(rstatix)
mshapiro_test(mvdata[,1:5])

which.max(D2)
mvdata=mvdata[-87,]
mvdata=mvdata[-71,]
#repeat above steps to check again

#if n>5000
#computationally taxing
library(QuantPsyc)
mult.norm(mvdata[,1:5])$mult.test

library(energy)
mvnorm.etest(mvdata[,1:5],R=50)

##check equality of variance covariance matrices
library(rstatix)
box_m(mvdata[,1:5],mvdata$Sex)
box_m(mvdata[,1:5],mvdata$OneorTwoparents)
#equality can be assumed

###Analysis
##MANOVA
manova=manova(cbind(mvdata$WSScommunication,mvdata$WSSAdaptability,
                    mvdata$WSSproblemsolving,mvdata$WSSworkethic,
                    mvdata$WSSteamwork)~mvdata$Sex*mvdata$OneorTwoparents)
overallWilks=summary(manova,test="Wilks")
overallWilks
##DDA as post hoc
library(DiscriMiner)
ddaSex=desDA(mvdata[,1:5], mvdata$Sex,covar="total")
ddaSex

#standardized coefficients
zcoef=round(ddaSex$discrivar[-1,],3)
zcoef
#structure coefficients
str_coef=round(ddaSex$discor,3)
str_coef

#if we needed to get group centroids
#get means per group on all outcomes
library(DiscriMiner)
groupmeans=groupMeans(mvdata[,1:5], mvdata$Sex)
groupmeans
centSexDF1_Female=zcoef[1]*groupmeans[1,1]+zcoef[2]*groupmeans[2,1]+
  zcoef[3]*groupmeans[3,1]+zcoef[4]*groupmeans[4,1]+zcoef[5]*groupmeans[5,1]
centSexDF1_Male=zcoef[1]*groupmeans[1,2]+zcoef[2]*groupmeans[2,2]+
  zcoef[3]*groupmeans[3,2]+zcoef[4]*groupmeans[4,2]+zcoef[5]*groupmeans[5,2]
centSexDF1=c(centSexDF1_Female,centSexDF1_Male)
names(centSexDF1)=c("Female","Male")
#save discriminant scores to run anova for sig group differences
mvdata$scores=ddaSex$scores
postanova=aov(scores~Sex,data=mvdata)

##final output info
#overall fit
overallWilks
#significance
ddaSex$values

#which DVs
zcoef
str_coef

#Which groups ##not needed here
centSexDF1
summary(postanova)
#effect size of group differences on discriminant function (still useful)
library(rstatix)
cohens_d(mvdata,scores~Sex)
#males .60 standard deviations higher than females

#males higher, but all coeffs negative, so males show lower of everything
#primarily communication, but secondarily problem solving/work ethic
#adaptability and teamwork had credit stolen, would have had some contribution

#would repeat for other factor or interaction if meaningful

