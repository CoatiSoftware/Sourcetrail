package com.github.javaparser.symbolsolver.reflectionmodel.comparators;

import java.lang.reflect.Method;
import java.util.Comparator;

/**
 * @author Federico Tomassetti
 */
public class MethodComparator implements Comparator<Method> {

    @Override
    public int compare(Method o1, Method o2) {
        int compareName = o1.getName().compareTo(o2.getName());
        if (compareName != 0) return compareName;
        int compareNParams = o1.getParameterCount() - o2.getParameterCount();
        if (compareNParams != 0) return compareNParams;
        for (int i = 0; i < o1.getParameterCount(); i++) {
            int compareParam = new ParameterComparator().compare(o1.getParameters()[i], o2.getParameters()[i]);
            if (compareParam != 0) return compareParam;
        }
        int compareResult = new ClassComparator().compare(o1.getReturnType(), o2.getReturnType());
        if (compareResult != 0) return compareResult;
        return 0;
    }
}
