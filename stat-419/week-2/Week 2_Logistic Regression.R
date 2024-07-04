####Lab 12 - Logistic Regression
###install packages
install.packages("aod")
install.packages("ggplot2")
install.packages("Hmisc")
library(aod)
library(ggplot2)
library(Hmisc)
###using the diabetes dataset from Hdata
getHdata(diabetes,"all")
attach(diabetes)
#select variables for analysis
myvars=c("glyhb","location","gender","frame")
data=diabetes[myvars]
#remove missing values from analysis data - listwise deletion
data2=na.omit(data)
#recode outcome variable - glyhb > 7 --> diabetic; < 7 not diabetic
data2$Diabetic=ifelse(data2$glyhb>7, c("1"),c("0"))
#make sure R recognizes categorical variables
data2$location=factor(data2$location)
data2$gender=factor(data2$gender)
data2$frame=factor(data2$frame)
data2$Diabetic=factor(data2$Diabetic)

###use generalized linear model function to fit logistic regression
##model coefficients and deviance/AIC
m1=glm(Diabetic~location+gender+frame,data=data2,family="binomial")
summary(m1)
#only frame large significant predictor
#get coefficient confidence intervals for alpha = .05
confint.default(m1)

##overall model fit
options(scipen=999)
with(m1,pchisq(null.deviance-deviance, df.null-df.residual, lower.tail=FALSE))

#model not significant... try without the nonsignificant predictors
m2=glm(Diabetic~frame,data=data2,family="binomial")
summary(m2)
with(m2,pchisq(null.deviance-deviance, df.null-df.residual, lower.tail=FALSE))
#now significant
#note that AIC was higher for m1 than m2, so worse model fit
#can compare m1 and m2 same as we have with previous regressions
anova(m2,m1)
#only deviance, no significance - can use textbook to get chi-square significance

##pseudo r-squared values
#Cox and Snell
coxsnell=1-exp((m2$deviance-m2$null.deviance)/379)
coxsnell
#Nagelkerke
nag=coxsnell/(1-(exp(-(m2$null.deviance/379))))
nag
#according to Cohen (1988) rules of thumb for r-squared, the effect size is small-moderate

##odds ratios with confidence intervals
exp(cbind(OR=coef(m2),confint(m2)))
#only frame large doesn't cross null (OR of 1)
#compared to small frame, large frame over 3 times as likely to develop diabetes

##structure coefficients
yhat = predict(m2,newdata=data2, type="response")
#need each category for frame, so dummy code and save in data
install.packages("fastDummies")
library(fastDummies)
data2=dummy_cols(data2,select_columns="frame")
structure_data=cbind(yhat,data2$frame_medium,data2$frame_large)
structuretable=cor(structure_data)
structure_coef=structuretable[,1]
structure_coef=structure_coef[-1]
structure_coef
sq_structure_coef=structure_coef^2
sq_structure_coef
#examine along with significance and odds ratios
summary(m2)
exp(cbind(OR=coef(m2),confint(m2)))
#multicollinearity didn't appear to cause issues since
#structure coefficients consistent with model summary
