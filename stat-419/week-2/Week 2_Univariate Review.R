#########Univariate statistics in R
#import RelationshipRelationshipData from Canvas and call "RelationshipData"
attach(RelationshipData)

###two sample t-tests in r
##independent samples t-test
table(ParentsMarried)
summary(ClosenessSum)

install.packages("car")
library(car)
leveneTest(ClosenessSum~as.factor(ParentsMarried),data=RelationshipData)
#equal variances assumed

t.test(ClosenessSum~ParentsMarried, data=RelationshipData, paired=FALSE, var.equal=TRUE, conf.level=.95)
#didn't have to have as.factor() for 0/1 varriable
#statistically significant difference!

#effect size
install.packages("effsize")
library(effsize)
cohen.d(ClosenessSum~ParentsMarried, RelationshipData=RelationshipData, paired=FALSE, hedges.correction=FALSE,
        conf.level=.95)

##paired samples t-test
summary(ReligiositySum_T1)
summary(ReligiositySum_T2)
t.test(ReligiositySum_T1, ReligiositySum_T2, paired=TRUE)
#note that these variables are in "wide" format - need formula approach if stacked/long format

#effect size
library(effsize)
cohen.d(ReligiositySum_T1, ReligiositySum_T2, paired=TRUE, hedges.correction=FALSE,
        conf.level=.95)
#what could be going wrong?
cohen.d(ReligiositySum_T1, ReligiositySum_T2, paired=TRUE, hedges.correction=FALSE,
        conf.level=.95, na.rm=TRUE)

###Multiple group comparison
#one way ANOVA
table(Ethnicity)
leveneTest(ClosenessSum~as.factor(Ethnicity), RelationshipData=RelationshipData)
#equal variances assumed
m1=aov(ClosenessSum~as.factor(Ethnicity), RelationshipData=RelationshipData)
summary(m1)

#if equal variances not assumed
m1b=oneway.test(ClosenessSum~as.factor(Ethnicity), RelationshipData=RelationshipData,var.equal=FALSE)
m1b

#check normality
install.packages("ggpubr")
library(ggpubr)
lm1=lm(ClosenessSum~as.factor(Ethnicity), RelationshipData=RelationshipData)
ggqqplot(residuals(lm1))
#for a significance test...
install.packages("rstatix")
library(rstatix)
shapiro_test(residuals(lm1))

#check for outliers
x11()
plot(as.factor(Ethnicity), ClosenessSum)

#effect size
library(lsr)
etaSquared(m1)

#post hoc test
#only if overall ANOVA is significant!
#equal variances
TukeyHSD(m1)
#not equal variances
games_howell_test(RelationshipData=RelationshipData,ClosenessSum~Ethnicity)

White=na.omit(RelationshipData[Ethnicity==1,"ClosenessSum"])
Black=na.omit(RelationshipData[Ethnicity==2,"ClosenessSum"])
cohensD(White, Black)
#what went wrong?
cohensD(White$ClosenessSum, Black$ClosenessSum)
library(effsize)
cohen.d(White$ClosenessSum,Black$ClosenessSum)

#assumption plots
plot(m1)

##two-way ANOVA
#ethnicity and male
leveneTest(ClosenessSum~as.factor(Ethnicity), RelationshipData=RelationshipData)
#unequal variances (not enough RelationshipData for this example to do unequal)
leveneTest(ClosenessSum~Male,RelationshipData=RelationshipData)
#what went wrong?
leveneTest(ClosenessSum~as.factor(Male),RelationshipData=RelationshipData)
leveneTest(ClosenessSum~as.factor(Ethnicity):as.factor(Male),RelationshipData=RelationshipData)
#equal variances assumed
m2=aov(ClosenessSum~as.factor(Ethnicity)*as.factor(Male),RelationshipData=RelationshipData)
summary(m2)

#equal variances not assumed
library(rstatix)
welch_anova_test(RelationshipData=RelationshipData, ClosenessSum~as.factor(Ethnicity)*as.factor(Male))
#post-hoc not necessary - no significance

##repeated measures ANOVA
install.packages("RelationshipDatarium")
library(RelationshipDatarium)
RelationshipData("selfesteem")
install.packages("reshape2")
library(reshape2)
selfesteem_long=melt(selfesteem,id.vars="id",
                     measure.vars=c("t1","t2","t3"),
                     variable.name="Time",
                     value.name="SelfEsteem")
#convert to factors
is.numeric(selfesteem_long$id)
selfesteem_long$id=as.factor(selfesteem_long$id)
is.numeric(selfesteem_long$id)
#run model
m3=aov(SelfEsteem~Time+Error(id/Time),RelationshipData=selfesteem_long)
summary(m3)

library(sjstats)
effectsize::eta_squared(m3,alternative="two.sided")

#post-hoc tests - emmeans
install.packages("emmeans")
library(emmeans)
m3post=emmeans(m3, specs=pairwise~Time)
m3post
#can see all pairwise comparisons

#alternatively
m3b=aov(SelfEsteem~Time+Error(id),RelationshipData=selfesteem_long)
summary(m3b)
#notice the exact same results - we will use this for simplicity

#for full report - use sjstats
install.packages("pwr")
library(sjstats)
anova_stats(m3)

##split plot ANOVA
Treatment=c(0,0,0,0,0,1,1,1,1,1)
selfesteemSP=cbind(selfesteem,Treatment)
selfesteemSP_long=melt(selfesteemSP,id.vars=c("id","Treatment"),
                       measure.vars=c("t1","t2","t3"),
                       variable.name="Time",
                       value.name="SelfEsteem")
#make id factor
selfesteemSP_long$id=as.factor(selfesteemSP_long$id)
is.numeric(selfesteemSP_long$id)
#fit model
m4=aov(SelfEsteem~Time*Treatment+Error(id),RelationshipData=selfesteemSP_long)
summary(m4)

library(sjstats)
effectsize::eta_squared(m4, alternative="two.sided")

#interaction non-significant, but what if we wanted pairwise comparisons?
library(emmeans)
m2post=emmeans(m4, specs=pairwise~Time:Treatment)
m2post

##Two way ANOVA interaction plots

# Box plot with two factor variables
boxplot(SelfEsteem ~ Time * Treatment, RelationshipData=selfesteemSP_long, frame = FALSE,
        col = c("#00AFBB", "#E7B800"), ylab="Self Esteem")
# Two-way interaction plot
interaction.plot(x.factor = selfesteemSP_long$Time, trace.factor = selfesteemSP_long$Treatment,
                 response = selfesteemSP_long$SelfEsteem, fun = mean,
                 type = "b", legend = TRUE,
                 xlab = "Time", ylab="Self Esteem", trace.label = "Treatment",
                 pch=c(1,19), col = c("#00AFBB", "#E7B800"))
#Lines almost parallel, so basically no interaction effect

###categorical associations
#chi-squared test
#using RelationshipRelationshipData
chisq=chisq.test(Male,UsedDrugs)
chisq
Proportions=rbind(chisq$observed[1,]/sum(chisq$observed[1,]),chisq$observed[2,]/sum(chisq$observed[2,]))
colnames(Proportions)=c("NoDrugs","UsedDrugs")
rownames(Proportions)=c("Female","Male")

#effect size - essentially correlation coefficient
library(rcompanion)
#use crosstabs table rather than RelationshipData
cramerVFit(chisq$observed)

###Correlation and Simple Regression
##Correlation
#using RelationshipRelationshipData
summary(ClosenessSum)
summary(Age)
#conduct default pearson r correlation
cor.test(Age,ClosenessSum)
#change confidence level
cor.test(Age,ClosenessSum,conf.level=.90)
#make scatterplot
plot(Age,ClosenessSum)
#Add fit line to plot
abline(lm(ClosenessSum~Age))

#test for statistical power
library(pwr)
pwr.r.test(n=589,r=.1775,sig.level=.05)
#very high statistical power
#can check how big of a sample would be needed for certain power
pwr.r.test(n=NULL,r=.1775,sig.level=.05,power=.8)

##change to Spearman's rho
cor.test(Age,ClosenessSum,method="spearman")
#notice larger than before

##Simple regression
#try the same RelationshipData as regression
#note that the fit line we added is actually from simple regression
reg1 = lm(ClosenessSum~Age)
summary(reg1)
#unstandardized regression equation is therefore:
# ClosenessSum = 41.3862 - .6367(Age)
#create standardized model
zClosenessSum=scale(ClosenessSum)
zAge=scale(Age)
zreg1 = lm(zClosenessSum~zAge)
summary(zreg1)
#note that the intercept is essentially zero
#beta for Age is now -.1788 - the same as the pearson r!
plot(zAge,zClosenessSum)
abline(zreg1)

###multiple regression
#for simplicity, we will listwise delete in the RelationshipRelationshipData
OmitRelationshipData=na.omit(RelationshipData)
attach(OmitRelationshipData)
#interested in seeing if different things predict life condition of participant

m5=lm(LifeConditionSum~TalkMother+AdviceMother+ProudMother+SpecialMother+
        LoveMother+LovedByMother+Interest_FacebookBlogging+
        Interest_FashionMagazine+Interest_HipHop+Interest_MP3CellLaptop)
summary(m5)

#remember: to get beta weights, standardize all variables in model

zm5=lm(scale(LifeConditionSum)~scale(TalkMother)+scale(AdviceMother)+scale(ProudMother)+
         scale(SpecialMother)+scale(LoveMother)+scale(LovedByMother)+scale(Interest_FacebookBlogging)+
         scale(Interest_FashionMagazine)+scale(Interest_HipHop)+scale(Interest_MP3CellLaptop))
round(zm5$coefficients,3)

##structure coefficients
yhat=m5$fitted.values
structure_RelationshipData=cbind(yhat,TalkMother,AdviceMother,ProudMother,SpecialMother,
                     LoveMother,LovedByMother,Interest_FacebookBlogging,
                     Interest_FashionMagazine,Interest_HipHop,
                     Interest_MP3CellLaptop)
structuretable=cor(structure_RelationshipData)
structure_coef=structuretable[,1]
structure_coef=structure_coef[-1]
structure_coef
sq_structure_coef=structure_coef^2
sq_structure_coef
summary(m1)
round(zm5$coefficients,3)

##test assumptions
#linearity
plot(m5,1)
#homoscedasticity
plot(m5,3)
#install.packages("lmtest")
library(lmtest)
bptest(m5)
#normality of residuals
plot(m5,2)
