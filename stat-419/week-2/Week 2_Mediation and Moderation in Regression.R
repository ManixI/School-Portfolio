####Lab 11 - Mediation and Moderation
###Mediation
install.packages("mediation")
library(mediation)

#import data from University of Virginia example
data=read.csv('http://static.lib.virginia.edu/statlab/materials/data/mediationData.csv')
colnames(data)=c("Grades","SelfEsteem","Happiness")
#examine model where self-esteem mediates relationship between grades and happiness

#mediate() requires M~X and Y~X+M as well as IV (treat) and mediator
#bootstrapping is preferred method - default simulations is 1000
m1 = lm(SelfEsteem~Grades, data=data)
m2 = lm(Happiness~Grades+SelfEsteem, data=data)
#note that these are steps 2 and 3 from Baron & Kenny (1986) - finding paths a and b
mediation=mediate(m1,m2,treat="Grades",mediator="SelfEsteem")
summary(mediation)
##look for ACME - average causal mediation effects (indirect effect)
##for significance and estimate
#beneficial to also report confidence interval
#ADE is average direct effect
#total effect is sum of both indirect and direct effect

###moderation
#import "Interaction Data" from Canvas
#contains math SAT scores, measure for student-teacher relationship, and IQ
#Testing if student teacher relationship moderates between mSAT and IQ

#we will use hierarchical process to help
int1 = lm(mSAT~IQ+TSRI, data=Interaction_Data)
summary(int1)
#can interpet first model for "main effects"

int2 = lm(mSAT~IQ+TSRI+IQ*TSRI, data=Interaction_Data)
summary(int2)
#estimate for interaction term IQ:TSRI represents interaction effect
#note significance
anova(int1,int2)
#significant increase in model fit, so interaction adds to model

#for standardized model - also another way to specify
int3 = lm(scale(mSAT)~scale(IQ)*scale(TSRI),data=Interaction_Data)
summary(int3)

#categorize TSRI for plot
median(Interaction_Data$TSRI)
catTSRI=matrix(NA,nrow=nrow(Interaction_Data),ncol=1)
for(i in 1:nrow(Interaction_Data)){
  if(Interaction_Data$TSRI[i]<42) catTSRI[i]="Less"
  else catTSRI[i]="More"
}

median(Interaction_Data$IQ)
catIQ=matrix(NA,nrow=nrow(Interaction_Data),ncol=1)
for(i in 1:nrow(Interaction_Data)){
  if(Interaction_Data$IQ[i]<99) catIQ[i]="Less"
  else catIQ[i]="More"
}

#plot
interaction.plot(x.factor = catIQ, trace.factor = catTSRI,
                 response = Interaction_Data$mSAT, fun = mean,
                 type = "b", legend = TRUE,
                 xlab = "IQ", ylab="Math SAT Score",
                 pch=c(1,19), col = c("#00AFBB", "#E7B800"))
#plot shows that strong relationship between IQ and mSAT for higher TSRI
#also almost no relationship for the lower TSRI


##alternatively could use cut() function
cutTSRI=cut(Interaction_Data$TSRI,breaks=2)
cutIQ=cut(Interaction_Data$IQ,breaks=2)

interaction.plot(x.factor = cutIQ, trace.factor = cutTSRI,
                 response = Interaction_Data$mSAT, fun = mean,
                 type = "b", legend = TRUE,
                 xlab = "IQ", ylab="Math SAT Score",
                 pch=c(1,19), col = c("#00AFBB", "#E7B800"))
