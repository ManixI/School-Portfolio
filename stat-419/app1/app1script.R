attach(data)

for (i in 1:nrow(data)){
  if (data$jobs[i]!=0){
    data$HadJob[i]=1
  } else {
    data$HadJob[i]=0
  }
}

library(car)
# if true then var.equal=FALSE, ie. varience isn't same
# if p.value is significant ^^ else var.equal=TRUE
leveneTest(perform,as.factor(HadJob),data=data)
#2.b
# p < 0.05 so reject equal variance
t.test(perform~HadJob, data=data, paired=FALSE, var.equal=TRUE, conf.level=0.95)
# 

#2.c
library(effsize)
cohen.d(perform,HadJob, data=data, paired=FALSE, hedges.correction=FALSE,
        conf.level=.95)

#5
library(rcompanion)
cor.test(perform,extrav, method="pearson", conf.level=0.95)

#6
reg1 = lm(formula=perform~extrav, data=data)
summary(reg1)


zPreform=scale(perform)
zExtrav=scale(extrav)
zreg1 = lm(zPreform~zExtrav)
summary(zreg1)

