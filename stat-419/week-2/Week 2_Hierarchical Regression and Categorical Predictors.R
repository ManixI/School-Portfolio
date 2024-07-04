##hierarchical regression
#Example from Lewis (2007)
#Import excel file "Lewis (2007) Data"
#should be 80 observations of 6 variables
#variable blocks defined on sheet 2 in excel file
#college_success as outcome, demographics in block 1, faculty_interaction in block 2
m1 = lm(college_success~mother_ed+father_ed+socioeconomic, data=data)
summary(m1)
m2 = lm(college_success~mother_ed+father_ed+socioeconomic+faculty_interaction, data=data)
summary(m2)
anova(m1,m2)
#faculty interaction didn't add anything significant on top of demographics
#could do in order in line with Carlson & Wu (2012) --> variable of interest 1st
m1a = lm(college_success~faculty_interaction, data=data)
summary(m1a)
anova(m1a,m2)
#demographics add significant contribution on top of faculty interaction
#faculty interaction still able to account for almost 70% of variance in success by itself
#suggests heavy multicollinearity with demographics
#could try one at a time
alt1 = lm(college_success~mother_ed+father_ed,data=data)
summary(alt1)
anova(alt1,m1)
#parent education significant, and socioeconomic adds significant on top of that
alt2 = lm(college_success~mother_ed+father_ed+faculty_interaction,data=data)
summary(alt2)
anova(alt1,alt2)
#faculty interaction adds significant on top of parent education
alt3 = lm(college_success~socioeconomic,data=data)
summary(alt3)
alt4 = lm(college_success~socioeconomic+faculty_interaction,data=data)
summary(alt4)
anova(alt3,alt4)
#faculty interaction adds significant on top of socioeconomic status
#seems that parent education steals most of faculty interaction credit and socioeconomic steals the rest
#CONCLUSION: all factors important to predict, although faculty interaction overlaps a lot with demographics


##categorical predictors
#import RelationshipData
#get rid of missing values
RelationshipData=na.omit(RelationshipData)
attach(RelationshipData)
#use Ethnicity and ParentMaritalStatus to predict SchoolPerformance
table(Ethnicity)
#caucasian, african american, hispanic, two or more
table(ParentMaritalStatus)
#married, not currently married
cat1=lm(SchoolPerformance~as.factor(Ethnicity)+as.factor(ParentMaritalStatus))
summary(cat1)
#chooses reference category alphabetically/numerically
#nonmarried significantly lower schoolperformance
#note this is a small effect size - rsquared = .011 - nonsignificant model
#reduce to only marital status
cat2=lm(SchoolPerformance~as.factor(ParentMaritalStatus))
summary(cat2)
#notice that result is now significant - improved degrees of freedom and reduced measurement error!
