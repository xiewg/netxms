/**
 * NetXMS - open source network management system
 * Copyright (C) 2003-2010 Victor Kirhenshtein
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
package org.netxms.client.maps;

import org.netxms.base.NXCPMessage;

/**
 * Represents link between two elements on map
 */
public class NetworkMapLink
{
	// Link types
	public static final int NORMAL = 0;
	public static final int VPN = 1;
	
	private String name;
	private int type;
	private long element1;
	private long element2;
	private String connectorName1;
	private String connectorName2;

	/**
	 * @param name
	 * @param type
	 * @param element1
	 * @param element2
	 * @param connectorName1
	 * @param connectorName2
	 */
	public NetworkMapLink(String name, int type, long element1, long element2, String connectorName1, String connectorName2)
	{
		this.name = name;
		this.type = type;
		this.element1 = element1;
		this.element2 = element2;
		this.connectorName1 = connectorName1;
		this.connectorName2 = connectorName2;
	}

	/**
	 * @param linkType
	 * @param element1
	 * @param element2
	 * @param connectorName1
	 * @param connectorName2
	 */
	public NetworkMapLink(int type, long element1, long element2)
	{
		this.name = "";
		this.type = type;
		this.element1 = element1;
		this.element2 = element2;
		this.connectorName1 = "";
		this.connectorName2 = "";
	}
	
	/**
	 * Create link object from NXCP message
	 * 
	 * @param msg NXCP message
	 * @param baseId base variable ID
	 */
	public NetworkMapLink(NXCPMessage msg, long baseId)
	{
		name = msg.getVariableAsString(baseId + 1);
		type = msg.getVariableAsInteger(baseId);
		element1 = msg.getVariableAsInt64(baseId + 4);
		element2 = msg.getVariableAsInt64(baseId + 5);
		connectorName1 = msg.getVariableAsString(baseId + 2);
		connectorName2 = msg.getVariableAsString(baseId + 3);
	}
	
	/**
	 * Fill NXCP message with link data
	 * 
	 * @param msg NXCP message
	 * @param baseId base variable ID
	 */
	public void fillMessage(NXCPMessage msg, long baseId)
	{
		msg.setVariableInt16(baseId, type);
		msg.setVariable(baseId + 1, name);
		msg.setVariable(baseId + 2, connectorName1);
		msg.setVariable(baseId + 3, connectorName2);
		msg.setVariableInt32(baseId + 4, (int)element1);
		msg.setVariableInt32(baseId + 5, (int)element2);
	}

	/**
	 * @return the linkType
	 */
	public int getType()
	{
		return type;
	}

	/**
	 * @return first (left) element
	 */
	public long getElement1()
	{
		return element1;
	}

	/**
	 * @return second (right) element
	 */
	public long getElement2()
	{
		return element2;
	}

	/**
	 * @return first (left) connector name
	 */
	public String getConnectorName1()
	{
		return connectorName1;
	}

	/**
	 * @return second (right) connector name
	 */
	public String getConnectorName2()
	{
		return connectorName2;
	}

	/**
	 * @return the name
	 */
	public String getName()
	{
		return name;
	}
	
	/**
	 * Get label for display. If name is not null and not empty, label will have form
	 * name (connector1 - connector2)
	 * otherwise it will have form
	 * connector1 - connector2
	 * If any of connector names is null or empty, it will be replaced with string "<noname>".
	 * 
	 * @return display label or null for unnamed link 
	 */
	public String getLabel()
	{
		if (isUnnamed())
			return null;
		
		StringBuilder sb = new StringBuilder();
		if ((name != null) && !name.isEmpty())
		{
			sb.append(name);
			sb.append(" (");
		}
		
		sb.append(((connectorName1 != null) && !connectorName1.isEmpty()) ? connectorName1 : "<noname>");
		sb.append(" - ");
		sb.append(((connectorName2 != null) && !connectorName2.isEmpty()) ? connectorName2 : "<noname>");

		if ((name != null) && !name.isEmpty())
		{
			sb.append(')');
		}
		return sb.toString();
	}
	
	/**
	 * Check if this link is unnamed.
	 * 
	 * @return true if all names (link and both connectors) are null or empty
	 */
	public boolean isUnnamed()
	{
		return ((name == null) || name.isEmpty()) &&
		       ((connectorName1 == null) || connectorName1.isEmpty()) &&
		       ((connectorName2 == null) || connectorName2.isEmpty());
	}

	/* (non-Javadoc)
	 * @see java.lang.Object#equals(java.lang.Object)
	 */
	@Override
	public boolean equals(Object obj)
	{
		if (obj instanceof NetworkMapLink)
			return (((NetworkMapLink)obj).element1 == this.element1) &&
			       (((NetworkMapLink)obj).element2 == this.element2);
		return super.equals(obj);
	}
}
