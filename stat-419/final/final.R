attach(fdata)

mvdata = na.omit(fdata[,c("Gender", "Sideofhemiplegia", "GenderSide", "AHA.1", "AHA.2",
                   "Melbourne.1", "Melbourne.2", "Performance.1", "Performance.2")])
mvdata$Gender=as.factor(mvdata$Gender)
mvdata$Sideofhemiplegia=as.factor(mvdata$Sideofhemiplegia)
mvdata$GenderSide=as.factor(mvdata$GenderSide)

library(stats)
D2=mahalanobis(mvdata[,4:9],colMeans(mvdata[,4:9]),cov(mvdata[,4:9]))
plot(density(D2,bw=.5),
     main="Squared Mahalanobis distances");rug(D2)
qqplot(qchisq(ppoints(100),df=2),D2,
       main=expression("Q-Q plot of Mahalanobis"*~D^2*
                         "vs. quantiles of"*~chi[2]^2))
abline(0,1,col="red")

library(rstatix)
mshapiro_test(mvdata[,4:9])
#significant non-normality?
#lets say cutoff was .05

#Identify extreme Maholonobi's distance
which.max(D2)
#remove from data for analysis
mvdata2=mvdata[-32,]

D2=mahalanobis(mvdata2[,4:9],colMeans(mvdata2[,4:9]),cov(mvdata2[,4:9]))
plot(density(D2,bw=.5),
     main="Squared Mahalanobis distances");rug(D2)
qqplot(qchisq(ppoints(100),df=2),D2,
       main=expression("Q-Q plot of Mahalanobis"*~D^2*
                         "vs. quantiles of"*~chi[2]^2))
abline(0,1,col="red")


library(rstatix)
box_m(mvdata2[,4:9],mvdata2$GenderSide)

###Analysis
##MANOVA
manova=manova(cbind(mvdata$AHA.1,mvdata$Melbourne.1,
                    mvdata$Performance.1)~mvdata$Sideofhemiplegia*mvdata$Gender)
overallWilks=summary(manova,test="Wilks")
overallWilks

library(DiscriMiner)
ddaGender=desDA(mvdata[,4:9], 
                mvdata$Sideofhemiplegia,
                covar="total")
ddaGender

ddaSideofhemiplegia=desDA(mvdata[,4:9], 
                mvdata$Sideofhemiplegia,
                covar="total")
ddaSideofhemiplegia

ddaGenderSide=desDA(mvdata[,4:9], 
                          mvdata$GenderSide,
                          covar="total")
ddaGenderSide

#standardized coefficients
zcoef=round(ddaGenderSide$discrivar[-1,],3)
zcoef
#structure coefficients
str_coef=round(ddaGenderSide$discor,3)
str_coef

library(DiscriMiner)
groupmeans=groupMeans(mvdata[,4:9], mvdata$GenderSide)
groupmeans
centFL = (zcoef[1]*groupmeans[1,1] 
          + zcoef[2]*groupmeans[2,1]
          + zcoef[3]*groupmeans[3,1]
          + zcoef[4]*groupmeans[4,1]
          + zcoef[5]*groupmeans[5,1]
          + zcoef[6]*groupmeans[6,1])

centFR = (zcoef[1]*groupmeans[1,2] 
          + zcoef[2]*groupmeans[2,2]
          + zcoef[3]*groupmeans[3,2]
          + zcoef[4]*groupmeans[4,2]
          + zcoef[5]*groupmeans[5,2]
          + zcoef[6]*groupmeans[6,2])

centML = (zcoef[1]*groupmeans[1,3] 
          + zcoef[2]*groupmeans[2,3]
          + zcoef[3]*groupmeans[3,3]
          + zcoef[4]*groupmeans[4,3]
          + zcoef[5]*groupmeans[5,3]
          + zcoef[6]*groupmeans[6,3])

centMR = (zcoef[1]*groupmeans[1,4] 
          + zcoef[2]*groupmeans[2,4]
          + zcoef[3]*groupmeans[3,4]
          + zcoef[4]*groupmeans[4,4]
          + zcoef[5]*groupmeans[5,4]
          + zcoef[6]*groupmeans[6,4])

centGenderSideDF1=c(centFL, centFR, centML, centMR)
names(centGenderSideDF1)=c("FL", "FR", "ML", "MR")

mvdata$scores=ddaGenderSide$scores
postanova=aov(scores~GenderSide,data=mvdata)


##final output info
#overall fit
overallWilks
#significance
ddaGenderSide$values

#which DVs
zcoef
str_coef

#Which groups ##not needed here
centGenderSideDF1
summary(postanova)

library(rstatix)
cohens_d(mvdata,scores~GenderSide)


#q 10, gap in AHA score groupings
centFLaha = (zcoef[1]*groupmeans[1,1] 
          + zcoef[2]*groupmeans[2,1])

centFRaha = (zcoef[1]*groupmeans[1,2] )

centMLaha = (zcoef[1]*groupmeans[1,3] 
          + zcoef[2]*groupmeans[2,3])

centMRaha = (zcoef[1]*groupmeans[1,4] 
          + zcoef[2]*groupmeans[2,4])

centGenderSideDF1aha=c(centFLaha, centFRaha, centMLaha, centMRaha)
names(centGenderSideDF1aha)=c("FL", "FR", "ML", "MR")

centGenderSideDF1aha
#standardize variables
#zmvdata2=as.data.frame(cbind(scale(mvdata2$AHA.1),scale(mvdata2$AHA.2),
#                             scale(mvdata2$Melbourne.1),scale(mvdata2$Melbourne.2),
#                             scale(mvdata2$Performance.1),scale(mvdata2$Performance.2),
#                             mvdata2$GenderSide))
#colnames(zmvdata2)=c("GenderSide", "AHA.1", "AHA.2",
#                     "Melbourne.1", "Melbourne.2", "Performance.1", "Performance.2")
#summary(zmvdata2)
#sd(zmvdata2$selfconf)

#library(MASS)
#dda1=lda(zmvdata2[,4:9],grouping=zmvdata2$GenderSide)
#dda1
#plot(dda1)
#plot(dda1,dimen=1,type="both")

# do dda on GenderSide

interaction.plot(x.factor = mvdata2$Gender, trace.factor = mvdata2$Sideofhemiplegia,
                 response = mvdata2$score["z1"], fun = mean,
                 type = "b", ledgend = TRUE,
                 xlab = "Gender", ylab = "AHA", trace.lable = "Hemoph",
                 pch=c(1,19), col = c("#00AFBB", "#E7B800"))
