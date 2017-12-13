/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2017 Raden Solutions
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */
package org.netxms.ui.eclipse.objectmanager.propertypages.helpers;

import org.eclipse.jface.viewers.TableViewer;
import org.eclipse.jface.viewers.Viewer;
import org.eclipse.jface.viewers.ViewerComparator;
import org.eclipse.swt.SWT;
import org.eclipse.swt.widgets.TableColumn;
import org.netxms.client.objects.configs.RackPassiveElementConfigEntry;
import org.netxms.ui.eclipse.objectmanager.propertypages.RackPassiveElements;

/**
 * Comparator for rack attribute list
 */
public class RackPassiveElementComparator extends ViewerComparator
{   
   /* (non-Javadoc)
    * @see org.eclipse.jface.viewers.ViewerComparator#compare(org.eclipse.jface.viewers.Viewer, java.lang.Object, java.lang.Object)
    */
   @Override
   public int compare(Viewer viewer, Object e1, Object e2)
   {
      TableColumn sortColumn = ((TableViewer)viewer).getTable().getSortColumn();
      if (sortColumn == null)
         return 0;
      
      int rc;
      switch((Integer)sortColumn.getData("ID"))
      {
         case RackPassiveElements.COLUMN_NAME:
            rc = ((RackPassiveElementConfigEntry)e1).getName().compareToIgnoreCase(((RackPassiveElementConfigEntry)e2).getName());
            break;
         case RackPassiveElements.COLUMN_POSITION:
            rc = Integer.signum(((RackPassiveElementConfigEntry)e1).getPosition() - ((RackPassiveElementConfigEntry)e2).getPosition());
            break;
         case RackPassiveElements.COLUMN_TYPE:
            rc = Integer.signum(((RackPassiveElementConfigEntry)e1).getType().getValue() - ((RackPassiveElementConfigEntry)e2).getType().getValue());
            break;
         case RackPassiveElements.COLUMN_ORIENTATION:
            rc = Integer.signum(((RackPassiveElementConfigEntry)e1).getOrientation().getValue() - ((RackPassiveElementConfigEntry)e2).getOrientation().getValue());
         default:
            rc = 0;
            break;
      }
      int dir = ((TableViewer)viewer).getTable().getSortDirection();
      return (dir == SWT.UP) ? rc : -rc;
   }
   
}