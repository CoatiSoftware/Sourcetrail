package com.sourcetrail;

import com.github.javaparser.ast.visitor.GenericListVisitorAdapter;
import com.github.javaparser.ast.visitor.GenericVisitorAdapter;
import com.github.javaparser.ast.visitor.VoidVisitorAdapter;

import com.github.javaparser.ast.*;
import com.github.javaparser.ast.body.*;
import com.github.javaparser.ast.comments.Comment;
import com.github.javaparser.ast.expr.*;
import com.github.javaparser.ast.nodeTypes.NodeWithAnnotations;
import com.github.javaparser.ast.stmt.*;
import com.github.javaparser.ast.type.*;

import java.util.Optional;
import java.util.Stack;

import javax.annotation.Generated;

public abstract class AstVisitorAdapter extends GenericVisitorAdapter<Boolean, Void>
{
	private Stack<ReferenceKind> m_typeRefKind = new Stack<ReferenceKind>();
	
	protected ReferenceKind getTypeReferenceKind()
	{
		if (!m_typeRefKind.isEmpty())
		{
			return m_typeRefKind.peek();
		}
		return ReferenceKind.TYPE_USAGE;
	}
	
	//- Compilation Unit ----------------------------------
	
	@Override public Boolean visit(final CompilationUnit n, Void arg)
	{
		m_typeRefKind.push(ReferenceKind.TYPE_USAGE);
		Boolean result = super.visit(n, arg);
		m_typeRefKind.pop();
        return result;
	}

	//- Body ----------------------------------------------

	@Override public Boolean visit(ClassOrInterfaceDeclaration n, Void arg)
	{
		Boolean result;
        {
            result = n.getTypeParameters().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getMembers().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getName().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getAnnotations().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.INHERITANCE);
        {
            result = n.getExtendedTypes().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getImplementedTypes().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
        
        return null;
	}

	@Override public Boolean visit(EnumDeclaration n, Void arg)
	{
		Boolean result;
        {
            result = n.getEntries().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getMembers().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getName().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getAnnotations().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.INHERITANCE);
        {
            result = n.getImplementedTypes().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
		
        return null;
	}

	//- Type ----------------------------------------------

	@Override public Boolean visit(ClassOrInterfaceType n, Void arg)
	{
		Boolean result;
        {
            result = n.getName().accept(this, arg);
            if (result != null)
                return result;
        }
		// don't visit the qualifier here.
//        if (n.getScope().isPresent()) {
//            result = n.getScope().get().accept(this, arg);
//            if (result != null)
//                return result;
//        }
        {
            result = n.getAnnotations().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
        if (n.getTypeArguments().isPresent()) {
            result = n.getTypeArguments().get().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
		
        return null;
	}

	//- Expression ----------------------------------------

	@Override public Boolean visit(MethodCallExpr n, Void arg)
	{
		Boolean result;
        {
            result = n.getArguments().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getName().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getScope().isPresent()) {
            result = n.getScope().get().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
        if (n.getTypeArguments().isPresent()) {
            result = n.getTypeArguments().get().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
		
        return null;
	}
	
	@Override public Boolean visit(ObjectCreationExpr n, Void arg)
	{
		Boolean result;
        if (n.getAnonymousClassBody().isPresent()) {
            result = n.getAnonymousClassBody().get().accept(this, arg);
            if (result != null)
                return result;
        }
        {
            result = n.getArguments().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getScope().isPresent()) {
            result = n.getScope().get().accept(this, arg);
            if (result != null)
                return result;
        }
//        {
//            result = n.getType().accept(this, arg);
//            if (result != null)
//                return result;
//        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
        if (n.getTypeArguments().isPresent()) {
            result = n.getTypeArguments().get().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
		
        return null;
	}

	//- Statements ----------------------------------------

	@Override public Boolean visit(ExplicitConstructorInvocationStmt n, Void arg)
	{
		Boolean result;
        {
            result = n.getArguments().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getExpression().isPresent()) {
            result = n.getExpression().get().accept(this, arg);
            if (result != null)
                return result;
        }
        if (n.getComment().isPresent()) {
            result = n.getComment().get().accept(this, arg);
            if (result != null)
                return result;
        }

		m_typeRefKind.push(ReferenceKind.TYPE_ARGUMENT);
        if (n.getTypeArguments().isPresent()) {
            result = n.getTypeArguments().get().accept(this, arg);
            if (result != null)
                return result;
        }
		m_typeRefKind.pop();
		
        return null;
	}
}
