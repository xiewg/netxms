/**
 * 
 */
package org.netxms.nxmc.objectbrowser;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerFilter;
import org.netxms.client.NXCObject;
import org.netxms.nxmc.core.extensionproviders.NXMCSharedData;

/**
 * @author Victor
 *
 */
public class ObjectTreeFilter extends ViewerFilter
{
	private String filterString = null;
	private Map<Long, NXCObject> objectList = null;
	private NXCObject lastMatch = null;
	
	/* (non-Javadoc)
	 * @see org.eclipse.jface.viewers.ViewerFilter#select(org.eclipse.jface.viewers.Viewer, java.lang.Object, java.lang.Object)
	 */
	@Override
	public boolean select(Viewer viewer, Object parentElement, Object element)
	{
		if (objectList == null)
			return true;

		boolean pass = objectList.containsKey(((NXCObject)element).getObjectId());
		if (!pass && (((NXCObject)element).getNumberOfChilds() > 0))
		{
			Iterator<NXCObject> it = objectList.values().iterator();
			while(it.hasNext())
			{
				NXCObject obj = it.next();
				if (obj.isParent(((NXCObject)element).getObjectId()))
				{
					pass = true;
					break;
				}
			}
		}
		return pass;
	}
	
	
	/**
	 * Set filter string
	 */
	public void setFilterString(final String filterString)
	{
		boolean fullSearch = true;
		if (this.filterString != null)
			if (filterString.startsWith(this.filterString))
				fullSearch = false;
		this.filterString = filterString.isEmpty() ? null : filterString.toLowerCase();
		updateObjectList(fullSearch);
	}
	
	
	/**
	 * Update list of matching objects
	 */
	private void updateObjectList(boolean doFullSearch)
	{
		if (filterString != null)
		{
			if (doFullSearch)
			{
				NXCObject[] fullList = NXMCSharedData.getSession().getAllObjects();
				objectList = new HashMap<Long, NXCObject>();
				for(int i = 0; i < fullList.length; i++)
					if (fullList[i].getObjectName().toLowerCase().startsWith(filterString))
					{
						objectList.put(fullList[i].getObjectId(), fullList[i]);
						lastMatch = fullList[i];
					}
			}
			else
			{
				lastMatch = null;
				Iterator<NXCObject> it = objectList.values().iterator();
				while(it.hasNext())
				{
					NXCObject obj = it.next();
					if (!obj.getObjectName().toLowerCase().startsWith(filterString))
						it.remove();
					else
						lastMatch = obj;
				}
			}
		}
		else
		{
			objectList = null;
			lastMatch = null;
		}
	}


	/**
	 * Get last matched object
	 * @return Last matched object
	 */
	public final NXCObject getLastMatch()
	{
		return lastMatch;
	}
}
